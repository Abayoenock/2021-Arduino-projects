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
#include <SPI.h>
#include <RFID.h>
#define SS_PIN 10
#define RST_PIN 9
SoftwareSerial esp(2, 3);// the pins for arduino communication to esp RX and TX respectively
RFID rfid(SS_PIN, RST_PIN);
// Setup variables:
int serNum0;
int serNum1;
int serNum2;
int serNum3;
int serNum4;
int sensor1 = A0;// pins that the sensors are connected to 
int sensor2 = A1;
int sensor3 = A2;
int sensor4 = A3;
String card_num = "";// the variable to hold the cuard number 
int lot1 = 0; int lot2 = 0; int lot3 = 0; int lot4 = 0; // variables to hold the status of  the parking lots
unsigned int lasttime=0;
void setup()
{
  Serial.begin(9600);
  esp.begin(9600);
  SPI.begin();
  rfid.init();
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);

}

void loop()
{
  lot1 = lot2 = lot3 = lot4 = 0 ; // to initialise the varible back to zero
  card_num = "";
  unsigned int currenttime=millis();
  if(currenttime-lasttime>=15000){/* to initialise the variable back to zero after 15 seconds or you can c
    hange it as deisired, this prevents from submitting the card twice since it may temper with results */
    serNum0=serNum1=serNum2=serNum3=serNum4=0;
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
lasttime=millis();

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
      }
    }
  }

  rfid.halt();

  if (analogRead(sensor1) < 200) {
    lot1 = 1;
  }
  if (analogRead(sensor2) < 200) {
    lot2 = 1;
  }
  if (analogRead(sensor3) < 200) {
    lot3 = 1;
  }
  if (analogRead(sensor4) < 200) {
    lot4 = 1;
  }
  String gets_link = "";
  gets_link += "{\"card\":\"";
  gets_link += card_num + "\",\"sensors\":";
  gets_link += "[" + String(lot1) + "," + String(lot2) + "," + String(lot3) + "," + String(lot4) + "]" + "}"; 

  Serial.println(gets_link);
  esp.println(gets_link);//print the data to esp in a JSON format so that we can deserialize the data in the esp 
  delay(3000);

}
