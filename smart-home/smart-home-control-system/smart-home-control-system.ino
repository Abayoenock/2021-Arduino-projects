#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
// WiFi parameters
const char* ssid = "emmanuel.com";
const char* password = "1111111a";
HTTPClient http;
//host to send data
void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(3000);
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  digitalWrite(D0, HIGH);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);
  digitalWrite(D5, HIGH);
  digitalWrite(D6, HIGH);
  digitalWrite(D7, HIGH);
}
void loop() {
  String host = "http://smart-home.enlabs.rw/controler.php";
  const char* hosts;
  hosts = host.c_str();
  Serial.println(hosts);
  http.begin(hosts);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.GET();
  String payload = http.getString(); // get data from webhost continously
  String  input = payload;
  StaticJsonDocument<200> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
    return;
  }

  Serial.println(payload);

  JsonArray status = doc["status"];
  int status_0 = status[0]; 
  int status_1 = status[1]; 
  int status_2 = status[2]; 
  int status_3 = status[3];
  int status_4 = status[4]; 
  int status_5 = status[5]; 
  int status_6 = status[6]; 
  int status_7 = status[7]; 


  if (status_0 == 0)
  {
    digitalWrite(D0, HIGH);
  }
  else
  {
    digitalWrite(D0, LOW);
  }
  if (status_1 == 0)
  {
    digitalWrite(D1, HIGH);
  }
  else
  {
    digitalWrite(D1, LOW);
  }
  if (status_2 == 0)
  {
    digitalWrite(D2, HIGH);
  }
  else
  {
    digitalWrite(D2, LOW);
  }
  if (status_3 == 0)
  {
    digitalWrite(D3, HIGH);
  }
  else
  {
    digitalWrite(D3, LOW);
  }
  if (status_4 == 0)
  {
    digitalWrite(D4, HIGH);
  }
  else
  {
    digitalWrite(D4, LOW);
  }
  if (status_5 == 0)
  {
    digitalWrite(D5, HIGH);
  }
  else
  {
    digitalWrite(D5, LOW);
  }
  if (status_6 == 0)
  {
    digitalWrite(D6, HIGH);
  }
  else
  {
    digitalWrite(D6, LOW);
  }
  if (status_7 == 0)
  {
    digitalWrite(D7, HIGH);
  }
  else
  {
    digitalWrite(D7, LOW);
  }
  delay(500);
  http.end();
  delay(500);
}
