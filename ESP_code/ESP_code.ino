// Code to activate IFTTT applet if signal is received for a set amount of time
// CC licence Niek Rutten, 17-5-2022

//import required libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"

int startDelay = 2000;
int stopDelay = 5000;
int lightHour = 18;

int acceptorState;
unsigned long mark;
unsigned long rejectMark;

int metalState=0;
int activateHome=0;

const char* ssid = "****";
const char* password = "****";

// Domain Name with full URL Path for HTTP POST Request
// REPLACE WITH YOUR EVENT NAME AND API KEY - open the documentation: https://ifttt.com/maker_webhooks
const char* serverName = "http://maker.ifttt.com/trigger/dinner/with/key/***;

const char* ntpServer = "europe.pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 7200;

void setup() {
  Serial.begin(115200);
  pinMode(15, INPUT);
  pinMode(22, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  // Random seed is a number used to initialize a pseudorandom number generator
  randomSeed(analogRead(33));
}

void loop() {
    metalState=(digitalRead(15)); // check for signal
    switch(acceptorState){ // go into appropriate state based on signal
    case 0://initial state
      if(metalState==1){ //if signal is positive go to case 1
        acceptorState = 1;
        mark = millis();
      }
      if(metalState==0){//if signal is negative go to case 2
        acceptorState = 2;
        rejectMark = millis();
      }
      break;
    case 1://start counting for how long the signal is positive
      if(millis()- mark>startDelay){
        acceptorState = 3;//Value has been positive x time or more, go to case 3
      }
      if(metalState==0) {
        acceptorState = 0;//Value has changed, back to init state
      }
      break;
    case 2://
      if(millis()- rejectMark>stopDelay){
        acceptorState = 4;//Value has been negative x time or more, go to case 4
      }
      if(metalState==1) {
        acceptorState = 0;//Value has changed, back to init state
      }
      break;
    case 3://activate applets to activate lights and audio
      if(activateHome!=1){
      activateHome=1;
      Serial.println("home activated");
      if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

        struct tm timeinfo;
        if(!getLocalTime(&timeinfo)){
          Serial.println("Failed to obtain time");}
          
          if(timeinfo.tm_hour>lightHour){
          Serial.println(timeinfo.tm_hour);
          const char* serverName = "http://maker.ifttt.com/trigger/light_start/with/key/***";
          http.begin(client, serverName);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          String httpRequestData = "value1=" + String(random(40)) + "&value2=" + String(random(40)) + "&value3=" + String(random(40));
          int httpResponseCode = http.POST(httpRequestData);
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          http.end(); 
          }
          const char* serverName = "http://maker.ifttt.com/trigger/start/with/key/***";
          http.begin(client, serverName);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          String httpRequestData = "value1=" + String(random(40)) + "&value2=" + String(random(40)) + "&value3=" + String(random(40));
          int httpResponseCode = http.POST(httpRequestData);
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          http.end();
      }   
      }
      acceptorState=0;
      break;
    case 4://activate applets to turn off smart home systems
      if(activateHome!=0){
      /*if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

        struct tm timeinfo;
        if(!getLocalTime(&timeinfo)){
          Serial.println("Failed to obtain time");}
          
          if(timeinfo.tm_hour>lightHour){
          Serial.println(timeinfo.tm_hour);
          const char* serverName = "http://maker.ifttt.com/trigger/light_stop/with/key/cG0K6RtcGNwXp3bMFgWLma";
          http.begin(client, serverName);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          String httpRequestData = "value1=" + String(random(40)) + "&value2=" + String(random(40)) + "&value3=" + String(random(40));
          int httpResponseCode = http.POST(httpRequestData);
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          http.end(); 
          }
          const char* serverName = "http://maker.ifttt.com/trigger/stop/with/key/cG0K6RtcGNwXp3bMFgWLma";
          http.begin(client, serverName);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          String httpRequestData = "value1=" + String(random(40)) + "&value2=" + String(random(40)) + "&value3=" + String(random(40));
          int httpResponseCode = http.POST(httpRequestData);
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          http.end();
      }*/   
      activateHome=0;
      Serial.println("home deactivated");}
      acceptorState=0;
      break;
    }

  if(metalState==1){// reflect received signal in onboard led
    digitalWrite(22, LOW);
  }
  else{
    digitalWrite(22, HIGH);
  }
  delay(5);
}
