#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "###"
#define BLYNK_DEVICE_NAME "AURA"
#define BLYNK_AUTH_TOKEN "###"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

IRsend ir(4);  //IR at D2

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "###";  //SSID and PWD used both for Blynk and OTA
char pass[] = "###";

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
#define BSp V4
#define Colp V5
#define Bsync V6
#define Ssync V7
#define PartyM V8
//////////////////////

/////Support variables/////
int power=true;
int Br=30;
int S=1;
int pstat=0;
int dat;
int effect[4]={FLA,STR,FAD,SMO};
int e=0;
int Mode=0;
int color[16]={R,G,B,W,C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12};
int c=0;
///////////////////////////

BLYNK_WRITE(Powerp)
{
  dat=param.asInt();
  if(dat)
  {
    if(power)
      ir.sendNEC(OFF, 32);
    else
      ir.sendNEC(ON, 32);
    power=!power;
  }
}

BLYNK_WRITE(Warmp)
{
  dat=param.asInt();
  if(dat)
  {
    ir.sendNEC(C1, 32);
    for(int i=Br;i>9;i--)
    {
      ir.sendNEC(BDO, 32);
      delay(10);
    }
    Br=9;
    Blynk.virtualWrite(BSp,Br);
    Blynk.virtualWrite(Bsync,Br);
    Mode=0;
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
    Br=30;
    Blynk.virtualWrite(Bsync,Br);
    
    ir.sendNEC(STR, 32);
    for(int i=30;i>=1;i--)
    {
      ir.sendNEC(BDO, 32);
      delay(10);
    }
    S=1;
    Mode=1;
    Blynk.virtualWrite(Ssync,S);
    Blynk.virtualWrite(BSp,S);   
  }
}

BLYNK_WRITE(Effectp)
{
  dat=param.asInt();
  Blynk.virtualWrite(BSp,S);
  if(dat)
  {
    ir.sendNEC(effect[e], 32);
    e++;
    if (e>3)
      e=0;
    Mode=1;
  }
}

BLYNK_WRITE(BSp)
{
  dat=param.asInt();
  Serial.println(dat);
  if(Mode==0)
  {
  if(dat>Br)
  {
    for(int i=Br;i<=dat;i++)
    {
      ir.sendNEC(BUP, 32);
      delay(10);
    }
  }
  if(dat<Br)
  {
    for(int i=Br;i>=dat;i--)
    {
      ir.sendNEC(BDO, 32);
      delay(10);
    }
  }
  Br=dat;
  }

  if(Mode==1)
  {
  if(dat>S)
  {
    for(int i=S;i<=dat;i++)
    {
      ir.sendNEC(BUP, 32);
      delay(10);
    }
  }
  if(dat<S)
  {
    for(int i=S;i>=dat;i--)
    {
      ir.sendNEC(BDO, 32);
      delay(10);
    }
  }
  S=dat;
  }
}

BLYNK_WRITE(Colp)
{
  dat=param.asInt();
  Blynk.virtualWrite(BSp,Br);
  if(dat)
  {
    ir.sendNEC(color[c], 32);
    c++;
    if (c>15)
      c=0;
    Mode=0;
  }
}

BLYNK_WRITE(Bsync)
{
  Br=param.asInt();
}

BLYNK_WRITE(Ssync)
{
  S=param.asInt();
}

BLYNK_WRITE(PartyM)
{
  delay(1500);
  ir.sendNEC(SMO, 32);
  for(int i=S;i<=17;i++)
  {
      ir.sendNEC(BUP, 32);
      delay(10);
  }
  S=17;
  Blynk.virtualWrite(Ssync,S);
}

BLYNK_CONNECTED()
{ if(pstat==0)
  {
    Blynk.syncVirtual(Warmp,Bsync,Ssync);
    pstat++;
  }
}

void setup()
{ 
  Serial.begin(9600);
  Serial.println();
  Serial.println("               -丂𝐞𝐧𝐬𝐞 𝐎𝐒 v1.2 for Aura-");
  Serial.println("Booting up...");
  OTA();
  ir.begin();
  Blynk.begin(auth, ssid, pass);
  delay(400);
  ir.sendNEC(ON, 32);
  delay(100);
}

void loop()
{
  Blynk.run();
  ArduinoOTA.handle();
}



void OTA()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setHostname("Aura");
  ArduinoOTA.setPassword((const char *)"###");
  
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}
