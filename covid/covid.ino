#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
SoftwareSerial ss(D6, D7);
#define ONE_WIRE_BUS D5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
const char* ssid = "covid";
const char* password = "covid1234";
float heart_beat = 0;
float tempC = 0;
float tempF = 0;
float oxygen = 0;
double b_temp = 0;
double humidity = 0;
HTTPClient http;
//host to send data
const char* host = "http://covid-19.enlabs.rw/api.php";
unsigned int last_millis = 0;
int i = 0;
void setup()
{
  sensors.begin();
  Serial.begin(9600);
  ss.begin(9600);
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Powering on");
  delay(3000);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
   
  }

}
void loop() {

  body_temp();
  displaylcd();
  ShowData();


}
void body_temp() {
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  tempF = sensors.toFahrenheit(tempC);
  Serial.println(tempC);

}


void ShowData()
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
    heart_beat = doc["heart"];
    oxygen = doc["oxygen"];
    b_temp = doc["t"];
    humidity = doc["h"];

    Serial.println(heart_beat);
    Serial.println(oxygen);
    Serial.println(b_temp);
    Serial.println(humidity);
    send_sensor_data();

  }
  delay(200);
}
void send_sensor_data() {
  String url = host;
  url += "?b_temp=";
  url += b_temp;
  url += "&humidity=";
  url += humidity;
  url += "&oxygen=";
  url += oxygen;
  url += "&heart_beat=";
  url += heart_beat;
  url += "&temp=";
  url += tempC;
  http.begin(url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  String payload = http.getString(); // get data from webhost continously
  Serial.println(payload);
  delay(500);
  http.end();
}

void displaylcd() {
  unsigned int current_millis = millis();
  if (current_millis - last_millis >= 2000 ) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("H:");
    lcd.print(heart_beat);
    lcd.print("Bpm");
    lcd.setCursor(0, 1);
    lcd.print("SPO2:");
    lcd.print(oxygen);
    lcd.print("%" );

    last_millis = millis() - 1000;;

  }
  if (current_millis - last_millis >= 1000 ) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  BT: ");
    lcd.print(tempC);
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("RT: ");
    lcd.print(b_temp);
    lcd.print(" C");
    lcd.print("  H: ");
    lcd.print(humidity);
    lcd.print(" % ");

  }

}
