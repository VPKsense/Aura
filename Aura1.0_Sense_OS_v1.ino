#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

IRsend ir(4);  //IR at D2

char auth[] = "QXqpKFLay18EE3uprRwvKWydEFYtscd4";
char ssid[] = "2PoInT0";
char pass[] = "akhilesh";

/////IR Codes Define/////
#define ON 0xF7C03F
#define OFF 0xF740BF
#define BUP 0xF700FF
#define BDO 0xF7807F
#define R 0xF720DF
#define G 0xF7A05F
#define B 0xF7609F
#define W 0xF7E01F
#define C1 0xF710EF
#define C2 0xF7906F
#define C3 0xF750AF
#define C4 0xF730CF
#define C5 0xF7B04F
#define C6 0xF7708F
#define C7 0xF708F7
#define C8 0xF78877
#define C9 0xF748B7
#define C10 0xF728D7
#define C11 0xF7A857
#define C12 0xF76897
#define FLA 0xF7D02F
#define STR 0xF7F00F
#define FAD 0xF7C837
#define SMO 0xF7E817
/////////////////////////

/////V pin Define/////
#define Calibp V0
#define Powerp V1
#define Warmp V2
#define Effectp V3
//////////////////////

/////Support variables/////
int brightness=30;
int Speed=1;
int pstat=0;
int dat;
int effect[4]={FLA,STR,FAD,SMO};
int e=0;
///////////////////////////

BLYNK_WRITE(Powerp)
{
  dat=param.asInt();
  if(dat)
    ir.sendNEC(ON, 32);
  else
    ir.sendNEC(OFF, 32);
}

BLYNK_WRITE(Warmp)
{
  dat=param.asInt();
  if(dat)
  {
    ir.sendNEC(C1, 32);
    for(int i=brightness;i>9;i--)
    {
      ir.sendNEC(BDO, 32);
      delay(10);
    }
  }
}

BLYNK_WRITE(Calibp)
{
  dat=param.asInt();
  if(dat)
  {
    ir.sendNEC(W, 32);
    for(int i=1;i<=30;i++)
    {
      ir.sendNEC(BUP, 32);
      delay(10);
    }
    brightness=30;
    
    ir.sendNEC(STR, 32);
    for(int i=30;i>=1;i--)
    {
      ir.sendNEC(BDO, 32);
      delay(10);
    }
    Speed=1;
  }
}

BLYNK_WRITE(Effectp)
{
  dat=param.asInt();
  if(dat)
  {
    ir.sendNEC(effect[e], 32);
    Serial.println(effect[e]);
    e++;
    if (e>3)
      e=0;
  }
}

BLYNK_CONNECTED()
{ if(pstat==0)
  {
    Blynk.syncVirtual(Warmp);
    pstat++;
  }
  else
  {
    //Blynk.syncVirtual(//);
  }
}

void setup()
{
  Serial.begin(9600);
  ir.begin();
  Blynk.begin(auth, ssid, pass,IPAddress(188,166,206,43),8080);
  delay(400);
  ir.sendNEC(ON, 32);
  delay(100);
}

void loop()
{
  Blynk.run();
}
