/*
  smart restaturant 
  order food and drinks from the menus and be visible directly to the device :
 

  This copy  access to enlabs servers under the subdomain of http://restaurant.enlabs.rw
  
*/
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/*IMPORTANT NOTICE :
 
 1:You must first install the esp8266 board first copy this link http://arduino.esp8266.com/stable/package_esp8266com_index.json 
 2 :All these libraries must be includes , if you dont have them pleae consider downloading them to be able to complile and upload the program

*/
LiquidCrystal_I2C lcd(0x27, 20, 4);

// WiFi parameters
const char* ssid = "restaurant";// the wifi name your device will connect to
const char* password = "restaurant";// the wifi  password for the deveice to connect too
HTTPClient http;
//host to send data
const char* host = "http://restaurant.enlabs.rw/api.php";
unsigned int  lastupdate=0;
int cancel_button=D5;
int allow_button=D0;
int buzzer=D8;
int order_identifier=0;
void setup() {
  Serial.begin(9600);
pinMode(cancel_button,INPUT);
pinMode(allow_button,INPUT);
pinMode(buzzer,OUTPUT);
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Powering on");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart restautant ");
  lcd.setCursor(0, 1);
  lcd.print("ordering system ");
  delay(2000);
   lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print("network...... ");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
   
  }
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi ");
  lcd.setCursor(0, 1);
  lcd.print("Connnected ");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(2000);
  lcd.clear();
}
void loop() {
  
  ShowData();

}


void ShowData(){
  String url = host;
 
  http.begin(url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  String payload = http.getString(); // get data from webhost continously
  Serial.println(payload);
  String input = payload;
  StaticJsonDocument<410> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
    return;
  }
int order = doc["order"]; // 1
String location = doc["location"]; // "KG578st 13"
String menu_name = doc["menu_name"];
String name = doc["name"]; 
String date = doc["date"]; // "09-07-2021 15:11"
int order_id = doc["order_id"]; // 214556
order_identifier=order_id;

http.end();
unsigned int currentTime=millis();
if(currentTime-lastupdate>=4000){
  
  if(order==0){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("No order placed");
   tone(buzzer,0);
  
  }
  else{
    tone(buzzer,400,500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(name);
    lcd.setCursor(0,1);
    lcd.print(menu_name);
    lcd.setCursor(0,2);
    lcd.print(location);
    
    
    }
    lastupdate=millis();
    
  
  }

if(digitalRead(allow_button)==HIGH){
  lastupdate=millis();
  if(order==0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No order to ");
    lcd.setCursor(0,1);
    lcd.print("confirm ");
    }else{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("confirming ......");
      confirm_orders();
      
      }
  }
if(digitalRead(cancel_button)==HIGH){
  lastupdate=millis();
  if(order==0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No order to ");
    lcd.setCursor(0,1);
    lcd.print("cancel");
    }else{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Cancelling ......");
      cancel_orders();
      
      }
  }




  
}
void confirm_orders(){
  String url = host;
  url+="?t=confirm&id=";
  url+=order_identifier;
  Serial.println(url);
  http.begin(url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  String payload = http.getString(); // get data from webhost continously
  Serial.println(payload);
  String input = payload;
  StaticJsonDocument<32> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
    return;
  }
int success = doc["success"]; // 1
if(success==1){
  lastupdate=millis();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("you have successfuly");
  lcd.setCursor(0,1);
  lcd.print("confirmed this order");
  delay(2000);
  }
  else{
     lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("FAILED to confirm");
  lcd.setCursor(0,1);
  lcd.print("this order");
     delay(2000);
    
    }
delay(500);
  http.end();
}
void cancel_orders(){
  String url = host;
  url+="?t=cancel&id=";
  url+=order_identifier;
   Serial.println(url);
  http.begin(url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  String payload = http.getString(); // get data from webhost continously
  Serial.println(payload);
  String input = payload;
  StaticJsonDocument<32> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
    return;
  }
int success = doc["success"]; // 1
if(success==1){
  lastupdate=millis();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("you have successfuly");
  lcd.setCursor(0,1);
  lcd.print("cancelled this order");
   delay(2000);
  }
  else{
     lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("FAILED to cancel");
  lcd.setCursor(0,1);
  lcd.print("this order");
   delay(2000);
    
    }

  http.end();
}
