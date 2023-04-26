#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
int analogPin1 = A1; // Current sensor output
int analogPin2 = A2; // Current sensor output
int analogPin3 = A3; // Current sensor output
long int sensorValue1 = 0;  // variable to store the sensor value read
long int sensorValue2 = 0;  // variable to store the sensor value read
long int sensorValue3 = 0;  // variable to store the sensor value read
float voltage = 0;
float current1 = 0;
float current2 = 0;
float current3 = 0;
String msg = " ";
String lcd_msg = " ";
String lcd_msg1 = " ";
unsigned int current_millis;
unsigned int last_millis = 0;
unsigned int last_millis_d = 0;
unsigned int l_m = 0;

int buzzer = 10; // choose the pin for the LED
int relay = 9;
int mVperAmp = 185; // use 100 for 20A Module and 66 for 30A Module
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
SoftwareSerial gsm(2, 3); // RX, TX
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
void setup() {
  Serial.begin(9600);           //  setup serial
  pinMode(buzzer, OUTPUT); // declare buzzer as output
  pinMode(relay, OUTPUT); // declare relay as output
  pinMode(analogPin1, INPUT); // declare sensor as input
  pinMode(analogPin2, INPUT); // declare sensor as input
  pinMode(analogPin3, INPUT); // declare sensor as input
  gsm.begin(115200);   // Setting the baud rate of GSM Module
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Line fault ");
  lcd.setCursor(0, 1);
  lcd.print("detection");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing ..... ");
  gsm.println("AT");
  delay(1000);
  gsm.println("AT+CFUN=?");
  delay(2000);
  gsm.println("AT+CFUN=1");
}
void loop() {
  lcd_msg = "";
  msg = " A fault has been detected on :";
  Voltage = getVPP1();
  VRMS = (Voltage / 2.0) * 0.707; //root 2 is 0.707
  AmpsRMS = (VRMS * 1000) / mVperAmp;
  current1 = AmpsRMS;
  Serial.print(AmpsRMS);
  Serial.println(" Amps RMS 1");
  Voltage = getVPP2();
  VRMS = (Voltage / 2.0) * 0.707; //root 2 is 0.707
  AmpsRMS = (VRMS * 1000) / mVperAmp;
  current2 = AmpsRMS;
  Serial.print(AmpsRMS);
  Serial.println(" Amps RMS 2");
  Voltage = getVPP3();
  VRMS = (Voltage / 2.0) * 0.707; //root 2 is 0.707
  AmpsRMS = (VRMS * 1000) / mVperAmp;
  current3 = AmpsRMS;
  Serial.print(AmpsRMS);
  Serial.println(" Amps RMS3");

  current1 = current1 - 0.6;
  current2 = current2 - 0.6;
  current3 = current3 - 0.6;

  if ((current1 <= 0 && current2 <= 0 && current3 <= 0) || (current1 > 0 && current2 > 0 && current3 > 0)   ) {

    Serial.println("No fault detected ");
    lcd_msg1 = "No Fault ";
    lcd_msg = "detected";
    digitalWrite(relay, HIGH);
  }
  else {
    digitalWrite(relay, LOW);
    lcd_msg += "On:";
    if (current1 <= 0) {
      msg += "line 1,";
      lcd_msg += "L1,";
    }
    if (current2 <= 0) {
      msg += "line 2,";
      lcd_msg += " L2,";
    }
    if (current3 <= 0) {
      msg += "line 3,";
      lcd_msg += " L3";
    }
    msg += " Please go check out";
    lcd_msg1 = "Fault detected:";
    Serial.println(msg);
    SendTextMessage();
    while (1) {
      digitalWrite(buzzer, HIGH);
      delay(250);
      digitalWrite(buzzer, LOW);
      delay(250);
      lcd_display();
    }

  }
  lcd_display();
  Serial.print("current 1: ");
  Serial.print(current1);

  Serial.print("   current 2: ");
  Serial.print(current2);
  Serial.print("V");

  Serial.print("   Current 3: ");
  Serial.print(current3);
  Serial.println("A");

}
void SendTextMessage()
{
  current_millis = millis();
  Serial.println("Sending Text...");
  gsm.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(200);
  gsm.print("AT+CMGS=\"+250786657284\"\r");
  delay(300);
  gsm.print(msg); //the content of the message
  gsm.print("\r");
  delay(600);
  gsm.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(200);
  gsm.println();
  Serial.println("Text Sent.");
  delay(500);
  last_millis = millis();

}
void lcd_display() {
  current_millis = millis();
  if (current_millis - last_millis_d > 1000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lcd_msg1);
    lcd.setCursor(0, 1);
    lcd.print(lcd_msg);
    last_millis_d = millis();
  }
}
float getVPP1()
{
  float result;
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here

  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) //sample for 1 Sec
  {
    readValue = analogRead(analogPin1);
    // see if you have a new maxValue
    if (readValue > maxValue)
    {
      /*record the maximum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      /*record the minimum sensor value*/
      minValue = readValue;
    }
  }

  // Subtract min from max
  result = ((maxValue - minValue) * 5.23) / 1024.0;

  return result;
}
float getVPP2()
{
  float result;
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here

  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) //sample for 1 Sec
  {
    readValue = analogRead(analogPin2);
    // see if you have a new maxValue
    if (readValue > maxValue)
    {
      /*record the maximum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      /*record the minimum sensor value*/
      minValue = readValue;
    }
  }

  // Subtract min from max
  result = ((maxValue - minValue) * 5.23) / 1024.0;

  return result;
}
float getVPP3()
{
  float result;
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here

  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) //sample for 1 Sec
  {
    readValue = analogRead(analogPin3);
    // see if you have a new maxValue
    if (readValue > maxValue)
    {
      /*record the maximum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      /*record the minimum sensor value*/
      minValue = readValue;
    }
  }

  // Subtract min from max
  result = ((maxValue - minValue) * 5.23) / 1024.0;

  return result;
}
