#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
// Set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);
//GSM sim "14, 15"; // RX,TX for Arduino and for the module it's TXD RXD, they should be inverted
SoftwareSerial sim(14, 15); // RX,TX for Arduino and for the module it's TXD RXD, they should be inverted
String number = "+250786135953";
volatile int flow_frequency1; // Measures flow sensor pulses
volatile int flow_frequency2; // Measures flow sensor pulses
volatile int flow_frequency3; // Measures flow sensor pulses
// Calculated litres/hour
float vol1 = 0.0, l_minute1;
float vol2 = 0.0, l_minute2;
float vol3 = 0.0, l_minute3;
unsigned char flowsensor1 = 2; // Sensor Input
unsigned char flowsensor2 = 18; // Sensor Input
unsigned char flowsensor3 = 19; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;
const int relay =   5;
int green = 8;
unsigned long SMSSent = 0;
float diff_1;
float diff_2;
void flow1 () // Interrupt function
{
  flow_frequency1++;
}
void flow2 () // Interrupt function
{
  flow_frequency2++;
}
void flow3 () // Interrupt function
{
  flow_frequency3++;
}
void setup()
{

  pinMode(flowsensor1, INPUT);
  pinMode(flowsensor2, INPUT);
  pinMode(flowsensor3, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  digitalWrite(flowsensor1, HIGH); // Optional Internal Pull-Up
  digitalWrite(flowsensor2, HIGH); // Optional Internal Pull-Up
  digitalWrite(flowsensor3, HIGH); // Optional Internal Pull-Up
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(flowsensor1), flow1, RISING); // Setup Interrupt
  attachInterrupt(digitalPinToInterrupt(flowsensor2), flow2, RISING); // Setup Interrupt
  attachInterrupt(digitalPinToInterrupt(flowsensor3), flow3, RISING); // Setup Interrupt
  currentTime = millis();
  cloopTime = currentTime;
  lcd.init();
  //lcd.begin(20, 4);
  lcd.backlight();
  lcd.clear();
  delay(1000);
  // TURN ON THE BLACKLIGHT AND PRINT A MESSAGE.
  lcd.setCursor(0, 0);
  lcd.print("Powering on");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("system is ready!");
  delay(1000);
  lcd.clear();
  Serial.begin(9600);
  Serial.println("Wait few seconds...");
  delay(3000);
  Serial.println("System Started...");
  sim.begin(9600);
}
void loop ()
{
  diff_1 = vol1 - vol2;
  diff_2 = vol2 - vol3;
  lcd.setCursor(0, 0);
  lcd.print("Sensor 1:");
  lcd.setCursor(10, 0);
  lcd.print(vol1, 2);
  
  lcd.setCursor(0, 1);
  lcd.print("Sensor 2:");
  lcd.setCursor(10, 1);
  lcd.print(vol2, 2);
  
  lcd.setCursor(0, 2);
  lcd.print("Sensor 3:");
  lcd.setCursor(10, 2);
  lcd.print(vol3);
  lcd.setCursor(0, 3);
  lcd.print("dif_1:");
  lcd.setCursor(6, 3);
  lcd.print(diff_1, 1);
  lcd.setCursor(10, 3);
  lcd.print("dif_2:");
  lcd.setCursor(16, 3);
  lcd.print(diff_2, 1);
  relayon();
  if ( diff_1 <=0.08 && diff_2 <=0.08)
  {
    ledon();
  }
  else if (diff_1 > 0.1  )
  {
    ledoff();
    relayoff();
    lcd.setCursor(0, 0);
    lcd.print("error in range 1!");

    if (SMSSent == 0)
    {
      SendMessage1();
      SMSSent = 1;
      exit(1);
    }
  }
  else if (diff_1 <= 0.1 && diff_2 > 0.1  )
  {
    ledoff();
    relayoff();
    lcd.setCursor(0, 0);
    lcd.print("error in range 2!");
    if (SMSSent == 0)
    {
      SendMessage2();
      SMSSent = 1;
      exit(1);
    }
  }

  currentTime = millis();
  // Every second, calculate and print litres/hour
  if (currentTime >= (cloopTime + 1000))
  {
    cloopTime = currentTime; // Updates cloopTime
    if (flow_frequency1 != 0) {
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_minute1 = (flow_frequency1 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      Serial.print("Rate: ");
      Serial.print(l_minute1);
      Serial.println(" L/M");
      l_minute1 = l_minute1 / 60;
      vol1 = vol1 + l_minute1;
      Serial.println("Vol1:");
      Serial.print(vol1);
      Serial.print(" L");
      flow_frequency1 = 0; // Reset Counter
      Serial.print(l_minute1, DEC); // Print litres/hour
      Serial.println(" L/Sec");
    }
    else {
      Serial.println(" flow rate = 0 ");

      Serial.print("Rate: ");
      Serial.print( flow_frequency1 );
      Serial.print(" L/M");

      Serial.println("Vol1:");
      Serial.print(vol1);
      Serial.print(" L");
    }
    //--------------------------------------- the next sensor

    if (flow_frequency2 != 0) {
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_minute2 = (flow_frequency2 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      Serial.print("Rate: ");
      Serial.print(l_minute2);
      Serial.println(" L/M");
      l_minute2 = l_minute2 / 60;

      vol2 = vol2 + l_minute2;
      Serial.println("Vol2:");
      Serial.print(vol2);
      Serial.print(" L");
      flow_frequency2 = 0; // Reset Counter
      Serial.print(l_minute2, DEC); // Print litres/hour
      Serial.println(" L/Sec");
    }
    else {
      Serial.println(" flow rate = 0 ");

      Serial.print("Rate: ");
      Serial.print( flow_frequency2 );
      Serial.print(" L/M");

      Serial.println("Vol2:");
      Serial.print(vol2);
      Serial.print(" L");
    }
    //------------------- the third sensor------------------
    if (flow_frequency3 != 0) {
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_minute3 = (flow_frequency3 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      Serial.print("Rate: ");
      Serial.print(l_minute3);
      Serial.println(" L/M");
      l_minute3 = l_minute3 / 60;

      vol3 = vol3 + l_minute3;
      Serial.println("Vol3:");
      Serial.print(vol3);
      Serial.print(" L");
      flow_frequency3 = 0; // Reset Counter
      Serial.print(l_minute3, DEC); // Print litres/hour
      Serial.println(" L/Sec");
    }
    else {
      Serial.println(" flow rate = 0 ");

      Serial.print("Rate: ");
      Serial.print( flow_frequency3 );
      Serial.print(" L/M");

      Serial.println("Vol:");
      Serial.print(vol3);
      Serial.print(" L");
    }



    Serial.println("");
    Serial.println("------------------------------------------------------------");




  }
}

//CODES TO TURN ON LED
void ledon() {
  digitalWrite(green, HIGH);
}

//CODES TO TURN OFF LED
void ledoff() {
  digitalWrite(green, LOW);
}
//CODES TO TURN ON RELAY
void relayon() {
  digitalWrite(relay, HIGH);
}
//CODES TO TURN OFF RELAY
void relayoff() {
  digitalWrite(relay, LOW);
}

//SENDING SMS CODES
void SendMessage1()
{
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r");
  delay(1000);
  String SMS = "Hi, sms from GSM that there is a problem in range one(1)";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
}

void SendMessage2()
{
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r");
  delay(1000);
  String SMS = "Hi, sms from GSM that there is a problem in range two(2)";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
}
