#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo myservo;
// WiFi parameters
const char* ssid = "admin";// network name 
const char* password = "admin123";// network password 
HTTPClient http;
//host to send data
String serial_number = "M-946ELabs";
const char* host = "http://medical.enlabs.rw/test.php";
int push_button = D5;
String last_pill_time = "";
int taken = 0;
unsigned int miss_millis = 0;
int missed_pills = 0;
String missed_time = "";
int msg_type = 0;
unsigned int last_millis_s = 0;
String next_m = " Not set";
int buzzer = D7;
void setup() {
  Serial.begin(9600);
  myservo.attach(D6);
  myservo.write(30);
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
  lcd.print("network... ");
  pinMode(push_button, INPUT);
  pinMode(buzzer, OUTPUT);
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
  unsigned int current_milli_s = millis();
  if (current_milli_s - last_millis_s >= 2000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Next medication");
    lcd.setCursor(0, 1);
    lcd.print("at : ");
    lcd.print(next_m);
    last_millis_s = millis();
  }
  fetch_data();

}


void  fetch_data() {
  String url = host;
  url += "?d=" + serial_number;
  http.begin(url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  String payload = http.getString(); // get data from webhost continously
  Serial.println(payload);
  String input = payload;
  StaticJsonDocument<200> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
    return;
  }
  int pill = doc["pill"]; // 0
  int times = doc["times"]; // 0
  String  next = doc["next"]; // "05:33 PM"
  String  time_c = doc["time"]; // "05:33 PM"
  int qty = doc["qty"]; // 1
  if (next.length() > 0) {
    next_m = next;
  }
  else {
    next_m = " Not set";
  }
 if (last_pill_time != time_c) {
  taken=0;
 }
  unsigned int last_millis = millis();
  if (pill == 1) {
    while (1) {
      unsigned int current_milli_s = millis();
      if (taken == 0) {
        digitalWrite(buzzer, HIGH);
        delay(250);
        digitalWrite(buzzer, LOW);
        delay(250);
      }
      if (current_milli_s - last_millis_s >= 2000) {
        if (taken == 0) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Press to take ");
          lcd.setCursor(0, 1);
          lcd.print(times);
          lcd.print(" pills ");

        }
        else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Next medication");
          lcd.setCursor(0, 1);
          lcd.print("at : ");
          lcd.print(next_m);
        }
        last_millis_s = millis();
      }

      if (digitalRead(push_button) == HIGH) {
        if (last_pill_time == time_c) {
          Serial.println("Medication already taken  next medication will be taken at " + next);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Medication already");
          lcd.setCursor(0, 1);
          lcd.print("teken ");
          delay(2000);
          lcd.setCursor(0, 0);
          lcd.print("Next medication");
          lcd.setCursor(0, 1);
          lcd.print("at : ");
          lcd.print(next);


        } else {
          int loops = 0;

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("please take your");
          lcd.setCursor(0, 1);
          lcd.print("medication ");
          while (loops < times) {
            myservo.write(0);
            delay(2000);
            myservo.write(30);
            delay(2000);
            loops++;
            taken = 1;
          }
          msg_type = 1;
          send_msg();
          last_pill_time = time_c;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Thank you");
          delay(1000);
        }
      }
      unsigned int send_m_millis = millis();
      if (send_m_millis - last_millis >= 30000) {
        if (taken == 0) {
          msg_type = 0;
          send_msg();
        }
        break;
      }
      delay(250);
    }

  }
  delay(500);
  http.end();

}
void  send_msg() {

  String url = host;
  url += "?d=" + serial_number + "&t=send_mail&taken=" + msg_type;
  http.begin(url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  String payload = http.getString(); // get data from webhost continously
  Serial.println(payload);
  String input = payload;
  StaticJsonDocument<200> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
    return;
  }
  int success = doc["success"]; // 0
  int success1 = doc["success1"]; // 0
  if (success1 == 1) {
    Serial.println("email  sent successfuly");
  }
  else {
    Serial.println("Failed to send email ");
  }
  if (success == 1) {
    Serial.println("Report recorded sucessfuly");
  }
  else {
    Serial.println("Failed to record report ");
  }
  http.end();

}
