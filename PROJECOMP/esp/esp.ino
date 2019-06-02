#include <ESP8266WiFi.h>                                                   
#include <FirebaseArduino.h>
#include <ArduinoJson.h>                                               
#include <time.h>
#include <string.h>
//#include <SoftwareSerial.h>
//SoftwareSerial arduino(D9,D10);
unsigned long count1=0;
#define FIREBASE_HOST "laboratory-monitoring-d1caa.firebaseio.com"
#define FIREBASE_AUTH "T4LGKaSCBuoGOVTPGSkb4AMaP1WuR7I8pMbkC6zM"

#define WIFI_SSID "Vc não sabe nem eu"
#define WIFI_PASSWORD "joaozinho"

String  timestamp;
String  dataStream;
String   rec;

struct Data{
  float humidity;
  float temperature;
  float pressure;
  bool  gas;
};

Data reading;

DynamicJsonBuffer jsonBuffer(200);
//JsonObject& root = jsonBuffer.createObject();

void setup() {
  /*reading.temperature=25;
  reading.humidity=50;
  reading.pressure=12000;
  reading.gas=false;*/
  
  Serial.begin(115200);
  delay(1000);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

}

void loop() {
  if(Serial.available()){
    reading.temperature=Serial.readStringUntil(' ').toFloat();
    reading.humidity=Serial.readStringUntil(' ').toFloat();
    reading.pressure=Serial.readStringUntil(' ').toFloat();
    reading.gas=Serial.readStringUntil('f').toInt();
    uploadData(&reading);
  }
}

void uploadData(struct Data * data){
  
  time_t now;
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now);
  
  timestamp=timeinfo->tm_year+1900;
  timestamp+='-';
  timestamp+=timeinfo->tm_mon+1;
  timestamp+='-';
  timestamp+=timeinfo->tm_mday;
  timestamp+='T';
  timestamp+=timeinfo->tm_hour;
  timestamp+=':';
  timestamp+=timeinfo->tm_min;
  timestamp+=':';
  timestamp+=timeinfo->tm_sec;
  timestamp+='Z';

  JsonObject& root = jsonBuffer.createObject();
  root["temperature"]=data->temperature;
  root["humidity"]=data->humidity;
  root["atmPressure"]=data->pressure;
  root["gas"]=data->gas;
  root["tStamp"]=timestamp;
  Firebase.push("Data",root);
  
}

