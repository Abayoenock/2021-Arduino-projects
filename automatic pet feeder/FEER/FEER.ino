#include <DS3231.h>
#include <Servo.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;
const byte ROWS = 4;
const byte COLS = 4;
const int stepPin = 10;
const int dirPin = A3;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
DS3231 rtc(A4, A5);


int feed_time = 1000;       //define how fast stepper motor works
int turn_deg = 90;

String feed1;
String feed2;
char key[8];
int j = 0;
char button;

void setup() {

  rtc.begin();
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  Serial.begin(9600);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  myservo.attach(10);  // attaches the servo on pin 9 to the servo object
  myservo.write(0);
}

void loop() {

  lcd.setCursor(0, 0);
  lcd.print("Time:");
  lcd.print(rtc.getTimeStr());
  lcd.setCursor(0, 1);
  lcd.print("Date:");               //Date to display
  lcd.print(rtc.getDateStr());
  Serial.println(rtc.getTimeStr());
  Serial.println(rtc.getDateStr());

  button = keypad.getKey();

  Serial.println( button);

  if (button == 'A') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HH:MM:SS 1st");
    delay(500);
    j = 0;
    lcd.setCursor(j, 1);
    key[0] = keypad.waitForKey();
    lcd.print(key[0]); j++;
    lcd.setCursor(j, 1);
    key[1] = keypad.waitForKey();
    lcd.print(key[1]); j++;
    lcd.setCursor(j, 1);
    key[2] = ':';
    lcd.print(key[2]); j++;
    lcd.setCursor(j, 1);
    key[3] = keypad.waitForKey();
    lcd.print(key[3]); j++;
    lcd.setCursor(j, 1);
    key[4] = keypad.waitForKey();
    lcd.print(key[4]); j++;
    lcd.setCursor(j, 1);
    key[5] = ':';
    lcd.print(key[5]); j++;
    lcd.setCursor(j, 1);
    key[6] = keypad.waitForKey();
    lcd.print(key[6]); j++;
    lcd.setCursor(j, 1);
    key[7] = keypad.waitForKey();
    lcd.print(key[7]);
    key[8] = '\0';
    feed1 = String(key);
    delay(1000);
    lcd.clear();
  }

  if (button == 'B') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HH:MM:SS 2nd");
    delay(500);
    j = 0;
    lcd.setCursor(j, 1);
    key[0] = keypad.waitForKey();
    lcd.print(key[0]); j++;
    lcd.setCursor(j, 1);
    key[1] = keypad.waitForKey();
    lcd.print(key[1]); j++;
    lcd.setCursor(j, 1);
    key[2] = ':';
    lcd.print(key[2]); j++;
    lcd.setCursor(j, 1);
    key[3] = keypad.waitForKey();
    lcd.print(key[3]); j++;
    lcd.setCursor(j, 1);
    key[4] = keypad.waitForKey();
    lcd.print(key[4]); j++;
    lcd.setCursor(j, 1);
    key[5] = ':';
    lcd.print(key[5]); j++;
    lcd.setCursor(j, 1);
    key[6] = keypad.waitForKey();
    lcd.print(key[6]); j++;
    lcd.setCursor(j, 1);
    key[7] = keypad.waitForKey();
    lcd.print(key[7]);
    key[8] = '\0';
    feed2 = String(key);
    delay(1000);
    lcd.clear();
  }
  if (button == 'D') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1st feed Time: ");
    lcd.setCursor(0, 1);
    lcd.print(feed1);               //Date to display
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("2nd feed Time: ");
    lcd.setCursor(0, 1);
    lcd.print(feed2);
    delay(2000);
    lcd.clear();
  }
  if (button == 'C') {
    myservo.write(turn_deg);
    delay(feed_time);
    myservo.write(0);


  }
  if (feed1 == rtc.getTimeStr())
  {
    myservo.write(turn_deg);
    delay(feed_time);
    myservo.write(0);
    delay(1000);
  }
  if (feed2 == rtc.getTimeStr())
  {
    myservo.write(turn_deg);
    delay(feed_time);
    myservo.write(0);
    delay(1000);
  }
}
