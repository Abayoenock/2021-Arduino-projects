/*
  RFID    ESP8266
  VCC====> 3V
  GND=====>GND
  RST====>D1
  SDA=====>D2
  SCL=====>D5
  MOSI=====>D7
  MISO=====>D6
  IRQ ===> Nta pin uyicomekaho uyirekeraho ubusa
*/
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <RFID.h>
#include <Servo.h>
Servo myservo;  // create servo object to control a servo
const char* ssid = "e_connect";// network name 
const char* password = "1111111a";// network password
const char* host = "http://192.168.43.214/digitalparking/device_api.php";// chnage the ip to your own ip address =>ipconfig

#define RST_PIN         0          // Configurable, see typical pin layout above 
#define SS_PIN          2         // Configurable, see typical pin layout above
HTTPClient http;
WiFiClient WiFiclient;
LiquidCrystal_I2C lcd(0x27, 16, 2);

RFID rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance
unsigned int lasttime = 0;
int serNum0;
int serNum1;
int serNum2;
int serNum3;
int serNum4;
String card_num = "";
unsigned int last_millis = 0;
int buzzer_pin = 16;
int buzzer_times = 0;
void setup() {
  Serial.begin(9600);		// Initialize serial communications with the PC
  SPI.begin();
  rfid.init();
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Powering on");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("connecting to ");
  lcd.setCursor(0, 1);
  lcd.print("network... ");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  pinMode(buzzer_pin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(2000);
  lcd.clear();
  myservo.attach(D8);  // attaches the servo on GIO2 to the servo object
}

void loop() {
  if (millis() - last_millis >= 1000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tap your card !");
    last_millis = millis();
  }
  card_num = "";
  unsigned int currenttime = millis();
  if (currenttime - lasttime >= 10000) {
    /* to initialise the variable back to zero after 2 seconds or you can
      change it as deisired, this prevents from submitting the card twice since it may temper with results */
    serNum0 = serNum1 = serNum2 = serNum3 = serNum4 = 0;
  }

  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      if (rfid.serNum[0] != serNum0
          && rfid.serNum[1] != serNum1
          && rfid.serNum[2] != serNum2
          && rfid.serNum[3] != serNum3
          && rfid.serNum[4] != serNum4
         ) {
        /* With a new cardnumber, show it. */
        Serial.println(" ");
        Serial.println("Card found");
        serNum0 = rfid.serNum[0];
        serNum1 = rfid.serNum[1];
        serNum2 = rfid.serNum[2];
        serNum3 = rfid.serNum[3];
        serNum4 = rfid.serNum[4];

        card_num += String(rfid.serNum[0], HEX) ;
        card_num += String(rfid.serNum[1], HEX) ;
        card_num += String(rfid.serNum[2], HEX) ;
        card_num += String(rfid.serNum[3], HEX) ;
        card_num += String(rfid.serNum[4], HEX) ;
        Serial.println(card_num);
        lasttime = millis();
        buzzer_times = 1;
        digitalWrite(buzzer_pin, HIGH);
        delay(250);
        digitalWrite(buzzer_pin, LOW);
        delay(250);
        send_card_data();// function to send data to the webserver


      } else {
        /* If we have the same ID, just write a dot. */
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("This card was");
        lcd.setCursor(0, 1);
        lcd.print("Just tapped");
        digitalWrite(buzzer_pin, HIGH);
        delay(250);
        digitalWrite(buzzer_pin, LOW);
        delay(250);
        digitalWrite(buzzer_pin, HIGH);
        delay(250);
        digitalWrite(buzzer_pin, LOW);
        delay(250);


      }
    }
  }

  rfid.halt();
}

void send_card_data() {
  String url = host;
  url += "?card=";
  url += card_num;
  http.begin(WiFiclient,url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  String payload = http.getString(); // get data from webhost continously
  Serial.println(payload);
  String input = payload;
  StaticJsonDocument<230> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
    return;
  }
  int card_valid = doc["card_valid"]; // 1
  int bal = doc["bal"]; // 0
  int servo = doc["servo"]; // 0
  const char* name = doc["name"]; // "Philbert"
  int charged = doc["charged"]; // 0
  int book = doc["book"]; // 0
  int bal_valid = doc["bal_valid"]; // 0
  int in_out = doc["in_out"]; // 0
  if (card_valid == 0) {
    // the card is not valid
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Invalid card !");
    buzzer_times = 3;
    buzzer();
    delay(1000);

  }
  else {
    // when the card is valid
    if (book == 0) {
      // when the user has not yet booked
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("You have not  ");
      lcd.setCursor(0, 1);
      lcd.print("booked  ");
      buzzer_times = 2;
      buzzer();
       delay(1000);

    }
    else {

      if (in_out ==0) {
        // the case the user is entering
        if (servo == 1) {
          // when the user has boooked the servo turn to open the gate
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Welcome");
          lcd.setCursor(0, 1);
          lcd.print(name);
          delay(2000);
          myservo.write(90);
          delay(2000);
          myservo.write(0);
        }
      }
      else {
        if (bal_valid == 0) {
          // when the user has insufficent balance
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Insufficeint ");
          lcd.setCursor(0, 1);
          lcd.print("balance");
          buzzer_times = 2;
          buzzer();
          delay(1000);

        }
        else {
          // when the user has balance on the account
          if (servo == 1) {
            // when the user has met all the conditions
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Thank you");
            lcd.setCursor(0, 1);
            lcd.print(name);
            delay(2000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Charged :");
            lcd.setCursor(0, 1);
            lcd.print(charged);
            delay(2000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("balance :");
            lcd.setCursor(0, 1);
            lcd.print(bal);
            myservo.write(90);
            delay(2000);
            myservo.write(0);
          }

        }


      }


    }
  }
  http.end();
}
void buzzer() {
  int i = buzzer_times;
  while (i < 3) {
    digitalWrite(buzzer_pin, HIGH);
    delay(500);
    digitalWrite(buzzer_pin, LOW);
    delay(500);
    i++;
  }
}
