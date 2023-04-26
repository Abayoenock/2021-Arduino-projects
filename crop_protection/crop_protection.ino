#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
SoftwareSerial gsm(3, 2); // RX, TX
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
int buzzer = 10; // choose the pin for the LED
int inputPin = 8; // choose the input pin (for PIR sensor)
int pirState = LOW; // we start, assuming no motion detected
int val = 0; // variable for reading the pin status
unsigned int current_millis;
unsigned int last_millis = 0;
unsigned int l_m = 0;
String msg = "";
int check_sound = 0;
void setup() {
  pinMode(buzzer, OUTPUT); // declare LED as output
  pinMode(inputPin, INPUT); // declare sensor as input
  Serial.begin(9600);
  gsm.begin(115200);   // Setting the baud rate of GSM Module
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Crop protection");
  lcd.setCursor(0, 1);
  lcd.print("system");
  gsm.println("AT");
  delay(1000);
  gsm.println("AT+CFUN=?");
  delay(2000);
  gsm.println("AT+CFUN=1");
}

void loop() {
  current_millis = millis();
  if (current_millis - l_m > 30000) {
    check_sound = 0;

  }
  if (check_sound == 1)
  {
    tone(buzzer, 45, 200);
  }
  else {
    tone(buzzer, 0);
  }
  val = digitalRead(inputPin); // read input value
  if (val == HIGH) { // check if the input is HIGH
   
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      pirState = HIGH;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Animal detected !");
      msg = "An animal has been detected in your farm , you may consider checking on your crops";
      SendTextMessage();
      l_m = millis();
      check_sound = 1;
      tone(buzzer,450, 300); // turn LED ON

    }
  } else {
    digitalWrite(buzzer, LOW); // turn LED OFF
    if (pirState == HIGH) {
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No animal ");
      lcd.setCursor(0, 1);
      lcd.print("detected !");
      delay(2000);
    }

  }
}
void SendTextMessage()
{
  current_millis = millis();
  if (current_millis - last_millis > 30000) {
    Serial.println("Sending Text...");
    gsm.print("AT+CMGF=1\r"); // Set the shield to SMS mode
    delay(100);
    gsm.print("AT+CMGS=\"+250786135953\"\r");
    delay(200);
    gsm.print(msg); //the content of the message
    gsm.print("\r");
    delay(500);
    gsm.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
    delay(100);
    gsm.println();
    Serial.println("Text Sent.");
    delay(500);
    last_millis = millis();
  }
}
