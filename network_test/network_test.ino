/*
  network connectivity 
  scans  wifi networks sends them to the database for recording with there geographical locations , tests internet connection of the wifi networks :
  https://www.enlabs.rw
  modified 20 sept 2021
  by enlightenment labs 

  This copy is issued under the lisence of enightenment labs .
  and has access to enlabs servers under the subdomain of http://wifi.enlabs.rw
   email:info@enlabs.rw
         abayo.h.enock@enlabs.rw

  ====>to free host our projects , contact us on the lnk below
  http://www.enlabs.rw/projects
*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
/*IMPORTANT NOTICE :
 
 1:You must first install the esp8266 board first copy this link http://arduino.esp8266.com/stable/package_esp8266com_index.json 
 2 :All these libraries must be includes , if you dont have them pleae consider downloading them to be able to complile and upload the program

*/

LiquidCrystal_I2C lcd(0x27, 16, 2);
static const int RXPin = D7, TXPin = D8;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);
// The TinyGPS++ object
TinyGPSPlus gps;
String passwords = "admin123";//wifi password changes to your base network password
String ssids = "admin";// network name changes to your base network name 
String _ssid = "";
String _pwd = "";
String r_pwd = "";
String r_ssid = "";
String lati = "";
String longi = "";
String host = "http://wifi.enlabs.rw/api.php";// the web server and the file to handle the http request  ==> this can be replace by an ip address in case of localhost ex{http://192.168.12.30/api.php}
int scan = D0;
int connectiviy_t = D6;
WiFiClient wifi;
HTTPClient http;
int last_millis = 0;
void connect_base_network() {
  Serial.print("Connecting to ");
  Serial.println(ssids.c_str());
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print(ssids);
  // - need to convert String to char*
  WiFi.begin(ssids.c_str(), passwords.c_str());
  // connecting to WiFi network
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    count++;
    delay(500);
    Serial.print(".");
    if ( count > 10 ) {
      return;
    }
  }

  Serial.print("IP Address: ");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected");
  Serial.println(WiFi.localIP());
}
void connect_network() {
  int check_if_connect = 0;
  WiFi.begin(r_ssid.c_str(), r_pwd.c_str());
  // connecting to WiFi network
  int count = 0;
  while (WiFi.status() != WL_CONNECTED  ||  check_if_connect == 1) {
    int lastupdate = 0;
    int current_millis = millis();
    if (current_millis - lastupdate >= 30000) {
      check_if_connect = 1;
    }
    count++;
    delay(500);
    Serial.print(".");
    if ( count > 10 ) {
      return;
    }
  }
  if (check_if_connect == 0) {
    Serial.print("IP Address: ");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connected");
    delay(2000);
    update_connectivity();
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.print("check your network password");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("connection failed");
    lcd.setCursor(0, 0);
    lcd.print("check password");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("connectioning to");
    lcd.setCursor(0, 1);
    lcd.print("base network");
    delay(2000);
    connect_base_network();
  }
}
void connectivity_c() {// to scan netsorks to test for internet connection
  byte available_networks = WiFi.scanNetworks();
  if ( available_networks > 0 ) {
    for (int network = 0; network < available_networks; network++) {
      //delay(1000);

      long rssi = WiFi.RSSI(network);
      Serial.print("RSSI:");
      Serial.println(rssi);
      lcd.clear();
      lcd.setCursor(0, 0);
      //  display network name, signal Strength and battery level
      lcd.print("SSID ");
      lcd.print(WiFi.SSID(network));
      lcd.setCursor(0, 1);
      lcd.print("RSSI:");
      lcd.print(rssi);
      delay(2000);
      _ssid = WiFi.SSID(network);
      fetch_password();
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("internet check ");
    lcd.setCursor(0, 1);
    lcd.print(" fineshed ");
    delay(4000);
  } else {
    Serial.println("No Signal !");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No networks ");
    lcd.setCursor(0, 1);
    lcd.print("found");
    delay(4000);
  }
}
void fetch_password() {// to retreive the password form the database so that it can connect to the network and test for internet connection

  Serial.println("fetching data from the website...");
  String link = host + "?ssid=" + _ssid + "&t=password";
  http.begin(link);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.GET();
  String payload = http.getString(); // get data from webhost continously
  Serial.println(payload);
  String input = payload;
  StaticJsonDocument<128> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
    return;
  }
  int available = doc["available"]; // 1
  String password = doc["password"]; // "enlabs.rw"
  r_ssid = _ssid;
  r_pwd = password;
  if (available == 1) {
    Serial.println("Network found");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("connecting to");
    lcd.setCursor(0, 1);
    lcd.print(r_ssid);
    delay(2000);
    connect_network();
    update_connectivity();
  }
  else {
    Serial.println("network not registered");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(r_ssid);
    lcd.setCursor(0, 1);
    lcd.print("not registered");
    delay(2000);
  }
  Serial.println("closing connection");
  http.end();
}
void update_connectivity() {// to send the data to the web server to check if the network has internet connecion. if the hppt request has a responce , it means that the network has internet connection
  Serial.println("fetching data from the website...");
  String link = host + "?ssid=" + _ssid + "&t=internet";
  http.begin(link);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
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
  if (success == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connectivity ");
    lcd.setCursor(0, 1);
    lcd.print("test successful");
    delay(4000);

  }
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connectivity ");
    lcd.setCursor(0, 1);
    lcd.print("test failed");
    delay(4000);
  }
  Serial.println("closing connection");
  http.end();
}

void scan_function() {// to scan for vailable networks in the region
  byte available_networks = WiFi.scanNetworks();
  if ( available_networks > 0 ) {
    for (int network = 0; network < available_networks; network++) {
      //delay(1000);
      long rssi = WiFi.RSSI(network);
      Serial.print("RSSI:");
      Serial.println(rssi);
      lcd.clear();
      lcd.setCursor(0, 0);
      //  display network name, signal Strength and battery level
      lcd.print("SSID ");
      lcd.print(WiFi.SSID(network));
      lcd.setCursor(0, 1);
      lcd.print("RSSI:");
      lcd.print(rssi);
      delay(2000);
      _ssid = WiFi.SSID(network);
      _pwd = WiFi.RSSI(network);
      send_wifi_data();

    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan fineshed ");
    delay(4000);
  } else {
    Serial.println("No Signal !");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No networks ");
    lcd.setCursor(0, 1);
    lcd.print("found");
    delay(4000);
  }
}

void send_wifi_data() {// to send scaned networks to the webserver 
  Serial.println("sending data to website ...");
  String _status = _ssid;
  String link = host + "?ssid=" + _ssid + "&t=insert&rssi=" + _pwd+"&lat="+lati+"&long="+longi;
  http.begin(link);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
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
  if (success == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("network  ");
    lcd.setCursor(0, 1);
    lcd.print("registered");
    

  }
  else if(success==2){
     lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("network already  ");
    lcd.setCursor(0, 1);
    lcd.print("registered");
    }
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Failed to ");
    lcd.setCursor(0, 1);
    lcd.print("network");
  }
  delay(500);
  Serial.println("closing connection");
  http.end();
  delay(2000);
}
void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);

  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  delay(10);
  pinMode(scan, INPUT);
  pinMode(connectiviy_t, INPUT);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome  ");
  delay(2000);
 
  connect_base_network();
}

void loop() {
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      displayInfo();
      int current_millis_loop = millis();
      if (current_millis_loop - last_millis >= 2000) {
        last_millis = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("choose scan or ");
        lcd.setCursor(0, 1);
        lcd.print("internet test");
      }
      if (digitalRead(scan) == HIGH) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Entered scanning");
        lcd.setCursor(0, 1);
        lcd.print("mode");

        scan_function();// to scan the available networks
      }
      if (digitalRead(connectiviy_t) == HIGH) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Entered internet ");
        lcd.setCursor(0, 1);
        lcd.print("check mode");
        connectivity_c();// to check for internet connectivity
      }
    }

    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
      Serial.println(F("No GPS detected: check wiring."));
      while (true);
    }
  }
}
void displayInfo()
{// for gps geographyical locations 
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    lati = String(gps.location.lat(), 6);
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    longi = String(gps.location.lng(), 6);
  
  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.println();
}
