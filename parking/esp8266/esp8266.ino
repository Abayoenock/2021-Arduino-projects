/*
  smart car parking
  this system provides an effecient solution to parking and provides a way od digital payment by using card , this allow the client pay the exact amount
  of money of the amount of time in our parking slot
  https://www.enlabs.rw
  modified 20 sept 2021
  by enlightenment labs

  This copy is issued under the lisence of enightenment labs .
  and has access to enlabs servers under the subdomain of http://parking.enlabs.rw
   email:info@enlabs.rw
         abayo.h.enock@enlabs.rw

  ====>to your  projects , contact us on the lnk below
  http://www.enlabs.rw/projects
*/
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
SoftwareSerial ss(D5, D6);// to make the digital pins into TX and RX respectively
Servo myservo;
// WiFi parameters
const char* ssid = "admin";// wifi network name
const char* password = "admin123";// wifi password
HTTPClient http;
//host to send data
const char* host = "http://parking.enlabs.rw/api.php";// the link to the website that receives data and handles operations
int sensor1, sensor2 , sensor3, sensor4;// variables to hold the values of the sensors
String card_n = "";// the varialble to store the card number
void setup() {
  Serial.begin(9600);
  ss.begin(9600);// to initialize the baurd rate for the software serail pins
  myservo.attach(D7);
  pinMode(D3, OUTPUT); // the buzzer pins
  digitalWrite(D3, LOW);
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Powering on");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print("network ");

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.print(".");
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
  lcd.setCursor(0, 0);
  lcd.print("Tap your card !");
  datahandler();// call of the fuction to receieve and handle the data from the arduino

}
void  datahandler()
{
  String datas = "";
  while (ss.available() != 0) {
    datas = datas + String(char (ss.read()));
  }
  if (datas != "") {
    Serial.println(datas);
    String input = datas;
    StaticJsonDocument<200> doc;
    DeserializationError err = deserializeJson(doc, input) ;
    if (err) {
      Serial.print("ERROR:");
      Serial.print(err.c_str());
      return;
    }
    String card = doc["card"];
    card_n = card;
    sensor1 = doc["sensors"][0];// dserialize the data of the sensor from the recived data from the ardiono serail communication
    sensor2 = doc["sensors"][1];
    sensor3 = doc["sensors"][2];
    sensor4 = doc["sensors"][3];
    Serial.println(card);
    Serial.println(sensor1);
    Serial.println(sensor2);
    Serial.println(sensor3);
    Serial.println(sensor4);
    if (card == "") {
      
      send_sensor_data();// the call of the fuction to send the sensor status to the web application when there is no card tappped
    }
    else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Processing...");
      digitalWrite(D3, HIGH);
      delay(1000);
      digitalWrite(D3, LOW);
      send_card_data();// the call of function to send the data including the sensor values and the card number
    }

  }
  delay(1000);
}
void send_sensor_data() {
  String url = host;
  url += "?l1=";
  url += sensor1;
  url += "&l2=";
  url += sensor2;
  url += "&l3=";
  url += sensor3;
  url += "&l4=";
  url += sensor4;
  http.begin(url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  String payload = http.getString(); // get data from webhost continously
  Serial.println(payload);
  delay(500);
  http.end();
}
void send_card_data() {
  String url = host;
  url += "?card=";
  url += card_n;
  url += "&l1=";
  url += sensor1;
  url += "&l2=";
  url += sensor2;
  url += "&l3=";
  url += sensor3;
  url += "&l4=";
  url += sensor4;
  http.begin(url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  String payload = http.getString(); // get data from webhost continously
  Serial.println(payload);
  String input = payload;
  StaticJsonDocument<250> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
    return;
  }
  int card_valid = doc["card_valid"];
  int balance = doc["balance"];
  int bal = doc["bal"];
  int servo = doc["servo"];
  String name = doc["name"];
  int charged = doc["charged"];
  int space = doc["space"];

  if (card_valid == 0) {// the http response is card_valid=> 0 when the user places and invalid card number ==> card which is not registered in the system
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Invalid card !");
    int i = 0;
    while (i < 3) {
      digitalWrite(D3, HIGH);
      delay(500);
      digitalWrite(D3, LOW);
      delay(500);
      i++;
    }
    delay(500);

  }
  else {

    if (space == 0) { //the http response  space==>0 , when the parking lot is full
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Parking full !");
      int i = 0;
      while (i < 2) {
        digitalWrite(D3, HIGH);
        delay(500);
        digitalWrite(D3, LOW);
        delay(500);
        i++;
      }
      delay(1000);
      lcd.clear();
    }
    else {
      if (bal == 0) {//the http response  bal==>0 , when the client has insufficient balance on the account
       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Insufficient");
        lcd.setCursor(0, 1);
        lcd.print("balance !");
     
         int i = 0;
        while (i < 4) {
          digitalWrite(D3, HIGH);
          delay(500);
          digitalWrite(D3, LOW);
          delay(500);
          i++;
        }
           delay(1000);
        lcd.clear();
        

      }
      else {// tgis statement executes when there is available space and also when the user has balance on his/ her account
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hello");
        lcd.setCursor(0, 1);
        lcd.print(name);
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Charged :");
        lcd.setCursor(0, 1);
        lcd.print(charged);
        lcd.print(" Rwf");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Balance : ");
        lcd.setCursor(0, 1);
        lcd.print(balance);
        lcd.print(" Rwf");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Opening gate ....");
        

        if (servo == 1) {
          /*to open the gate ==> this uses a delay of 3 seconds can be changed as you desire,
            or you can use a sensor but in this case , we considered budget and wen weith a delay fuction*/

          myservo.write(180);
          delay(3000);
          myservo.write(0);
        }



      }

    }
  }
  Serial.println(card_valid);
  Serial.println(balance);
  Serial.println(bal);
  Serial.println(servo);
  Serial.println(name);
  Serial.println(charged);



  delay(500);
  http.end();
}
