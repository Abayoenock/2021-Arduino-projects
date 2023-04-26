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
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"
#include <SPI.h>
#include <RFID.h>
#define RST_PIN         5          // Configurable, see typical pin layout above 
#define SS_PIN          4         // Configurable, see typical pin layout above
/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "e_connect"
#define WLAN_PASS       "1111111a"
/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "abayo"
#define AIO_KEY         "aio_TCHv851JGgPgHH4XABnsOeuE8Kco"
/************* DHT11 Setup ********************************************/
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
/************ Global State (you don't need to change this!) ******************/
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
///****************************** Feeds ***************************************/
//// Setup a feed called 'photocell' for publishing.
//// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish door = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/lock_unlock");
/*************************** Sketch Code ************************************/
// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();
int checks = 0;

RFID rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance
unsigned int lasttime = 0;
int serNum0;
int serNum1;
int serNum2;
int serNum3;
int serNum4;
String card_num = "";
void setup() {

  Serial.begin(115200);
  SPI.begin();
  rfid.init();
  dht.begin();
  delay(10);
//  Serial.println(F("Adafruit MQTT demo"));
//  // Connect to WiFi access point.
//  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

}
uint32_t x = 0;
void loop() {
  card_read();
 temp_humidity();
  delay(500);
}

void temp_humidity() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  // Read humidity
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  //publish temperature and humidity
  Serial.print(F("\nTemperature: "));
  Serial.print(t);
  Serial.print(F("\nHumidity: "));
  Serial.print(h);
  temperature.publish(t);
  humidity.publish(h);
  door.publish(checks);

}
void card_read() {
  card_num = "";
  unsigned int currenttime = millis();
  if (currenttime - lasttime >= 2000) {
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
        lasttime = millis();

        do_action();// call of the function when the user places a card

        //Serial.println(" ");
        Serial.println("Cardnumber:");
        Serial.print("Dec: ");
        Serial.print(rfid.serNum[0], DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[1], DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[2], DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[3], DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[4], DEC);
        Serial.println(" ");

        Serial.print("Hex: ");
        Serial.print(rfid.serNum[0], HEX);
        Serial.print(", ");
        Serial.print(rfid.serNum[1], HEX);
        Serial.print(", ");
        Serial.print(rfid.serNum[2], HEX);
        Serial.print(", ");
        Serial.print(rfid.serNum[3], HEX);
        Serial.print(", ");
        Serial.print(rfid.serNum[4], HEX);
        Serial.println(" ");
      } else {
        /* If we have the same ID, just write a dot. */
        Serial.print(".");
        Serial.println("Card already tapped , wait for a while to tap your card again");
      }
    }
  }

  rfid.halt();
}
void do_action() {
  if (card_num == "[put your card number in hexadecimal structure]") {
    // code to execute when the card matches
    checks = 1;
     MQTT_connect();
    door.publish(checks);
    delay(4000);
    checks = 0;

  }
  else {
    Serial.println("Invalid card !");
    // code to execute when the user places a wrong card
    checks = 0;
  }

}
void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
