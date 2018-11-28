#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// OLED display TWI address
#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(-1);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const int sw1=9;
const int sw2=11;
const int relay=2;
const int volt=A7;
const int curr=A6;

const int vth=240;
const int ith=3;

const int Vd=0.7;
const int sens=185;
const double Iofc=0.06;
const double Vof=23;
const long R1=455000;
const long R2=2010;

const int vrij=200;

const double sq2=sqrt(2);
double V,I,P,Virms,Vioff,Iof;
bool nikad=false;

void setup() {
  //Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE);
  pinMode(sw1,INPUT_PULLUP);
  pinMode(sw2,INPUT_PULLUP);
  pinMode(relay,OUTPUT);
  digitalWrite(relay,1);
  delay(200);
  Virms=readac(curr)/2/sq2; 
  Vioff=(Virms*1000)-2500;
  Iof=Vioff/sens;
  delay(200);
}

void loop() {
  Virms=readac(curr)/2/sq2; 
  Vioff=(Virms*1000)-2500;
  I=Vioff/sens-Iof-Iofc;
  I=abs(I);
  V=1.0*readac(volt)*(R1+R2)/R2/2+2*Vd+Vof;
  if(V<195 and V>70 or I>ith or V>vth){
    digitalWrite(relay,0);
    //nikad=true;
  }
  if(V<190){
    V=0;
    //I=0;
  }
  P=V*I;
  oled();
  delay(300);
}

double readac(int pin){
  int temp;
  int mx = 0;
  int mn = 1024;
  unsigned long t=millis();
  while(millis()-t<vrij)
  {
    temp=analogRead(pin);
    if (temp>mx)
      mx=temp;
    if (temp<mn)
      mn=temp;
  }
  return 1.0*(mx - mn)*5/1023;
}

void oled(){
  display.setTextSize(2);
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("  BULBINO");
  display.setTextSize(1);
  display.print("\n\n\n ");display.print(V,0);display.print(" V     ");display.print(I);display.print(" A\n\n");
  display.setTextSize(2);
  display.print(P);
  display.print(" W");
  display.display();
}

void ser(){
  Serial.print("Virms = ");Serial.println(Virms);
  Serial.print("Vioff = ");Serial.println(Vioff);
  Serial.print("V = ");Serial.println(V);
  Serial.print("I = ");Serial.println(I);
  Serial.println();
}
