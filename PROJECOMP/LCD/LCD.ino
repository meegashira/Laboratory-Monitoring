#include <Adafruit_Sensor.h> 
#include <Adafruit_BMP280.h>
#include <LiquidCrystal.h>
#include <DHT.h>

//Constants
#define pushButton 10
#define gasSensor 13
#define warningLED 6
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//general declarations
Adafruit_BMP280 bmp; 
LiquidCrystal lcd(12,11,5,4,3,2);
/*-LCD pins conection-*/
/*1 .Vss_____GND      */
/*2 .Vdd_____5V       */
/*3 .V0______POT      */
/*4 .Rs______pin 12   */
/*5 .R/W_____GND      */
/*6 .Enable__pin 11   */
/*7 .DB0              */
/*8 .DB1              */
/*9 .DB2              */
/*10.DB3              */
/*11.DB4_____pin 5    */
/*12.DB5_____pin 4    */
/*13.DB6_____pin 3    */
/*14.DB7_____pin 2    */
/*15.Anodo___5V       */
/*16.Catodo__GND      */
/**********************/

//global variables
int lcdState=0;   //switch control
int btnState=0;   //switch control

float temperature;//Sensor data
float pressure;   //Sensor data
float humidity;   //Sensor data
bool  gas;        //Sensor data

unsigned long timer;  //time counter
unsigned long dataBaseBaud=60000;//upload data baud rate
String sendingStream; //String of data

void setup() {
  pinMode(pushButton, INPUT);
  pinMode(gasSensor,  INPUT);
  pinMode(warningLED, OUTPUT);
  lcd.begin(16,2);            //initializing lcd
  lcd.setCursor(0,0);         //seting cursor position
  dht.begin();                //initializing dht
  Serial.begin(115200);       //initializing serial com with esp28566
  Serial.println(dataBaseBaud);
  if(!bmp.begin(0x76)){       //initializing bpm280 if error do...
    digitalWrite(warningLED,HIGH);
    while(1);
  }
  timer=millis();
}

void loop() {    
    if(digitalRead(pushButton)){        //pushButtom reading
      delay(100);                       //debounce delay
      btnState=digitalRead(pushButton);
      if(btnState==0){                  //buttom released
        lcdState++;                     //next state
        if(lcdState>3) lcdState=0;//looping stat
        lcd.clear();
      }
      //digitalWrite(warningLED,LOW);
    }
    readSensors();                    //reading all sensor values
    if(millis()-timer>=dataBaseBaud){ //cheking data sending time
      sendData();                     //sending data to esp28566
      timer=millis();                 //refresh timer
    }
    switch(lcdState){
      case 0:
        lcd.setCursor(0,0);
        lcd.print("Temperatura"); 
        lcd.setCursor(0,1);
        lcd.print(temperature);   
        lcd.print("C");  
        break;
  
      case 1:
        lcd.setCursor(0,0);
        lcd.print("Existe Gas:");
        if(gas==1){
          lcd.setCursor(0,1);
          lcd.print("Nao");
        }else{
          lcd.setCursor(0,1);
          lcd.print("Sim");      
        }
        break;

      case 2:
        lcd.setCursor(0,0);
        lcd.print("Umidade");
        lcd.setCursor(0,1);
        lcd.print(humidity);
        lcd.print("%");
        break;      

      case 3:
        lcd.setCursor(0,0);
        lcd.print("Pressao");
        lcd.setCursor(0,1);
        lcd.print(pressure);
        lcd.print("Pa");
        break;

      default:
        break;
    }
}

void readSensors(){ //reading sensors
  temperature=bmp.readTemperature();
  pressure=bmp.readPressure();
  humidity = dht.readHumidity();
  gas = digitalRead(gasSensor);
}

void sendData(){
  sendingStream=""; //Concatenate data in a string
  sendingStream+=temperature;
  sendingStream+=" ";
  sendingStream+=humidity;
  sendingStream+=" ";
  sendingStream+=pressure;
  sendingStream+=" ";
  sendingStream+=gas;
  sendingStream+='f';//end message char
  Serial.print(sendingStream);//send data to esp28566
  /*Serial.println(millis()-timer);
  digitalWrite(warningLED,HIGH);*/
}

