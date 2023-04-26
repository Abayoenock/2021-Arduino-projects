#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
SoftwareSerial bluetooth(2, 3); // RX, TX
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
#define relay1 5
#define relay2 7
String voice_command;
unsigned int last_refresh = 0;
String lampone_status = "OFF";
String lamptwo_status = "OFF";

void setup()
{
  Serial.begin(9600);            //Set rate for communicating with phone
  bluetooth.begin(9600);
  pinMode(relay1, OUTPUT);       //Set pin  as an output
  pinMode(relay2, OUTPUT);       //Set pin as an output
  digitalWrite(relay1, HIGH);    // to provide a high state to switch off the lights
  digitalWrite(relay2, HIGH);
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Voice controlled");
  lcd.setCursor(0, 1);
  lcd.print("automation");
  delay(3000);
}

void loop()
{
  lcd_display();
  voice_command = "";
  while (bluetooth.available()) { //Check if there is an available byte to read
    delay(10); //Delay added to make thing stable
    char c = bluetooth.read(); //Conduct a serial read
    if (c == '#') {
      break; //Exit the loop when the # is detected after the word
    }
    voice_command += c; //Shorthand for voice = voice + c
  }

  if (voice_command.length() > 0)
  {
    Serial.println(voice_command);

    if (voice_command == "turn on light one" || voice_command == "lamp one on") {
      digitalWrite(relay1, LOW);
      lampone_status = "ON";

    }
    else if (voice_command == "turn off light one" || voice_command == "lamp one off") {
      digitalWrite(relay1, HIGH);
      lampone_status = "OFF";
    }
    else if (voice_command == "turn on light 2" || voice_command == "lamp two on") {
      digitalWrite(relay2, LOW);
      lamptwo_status = "ON";
    }
    else if (voice_command == "turn off light two" || voice_command == "lamp two off") {
      digitalWrite(relay2, HIGH);
      lamptwo_status = "OFF";
    }
    else if (voice_command == "turn on all lights" || voice_command == "lamps on") {
      digitalWrite(relay1, LOW);
      digitalWrite(relay2, LOW);
      lamptwo_status = lampone_status = "ON";
    }
    else if (voice_command == "turn off all lights" || voice_command == "lamps off") {
      digitalWrite(relay1, HIGH);
      digitalWrite(relay2, HIGH);
      lamptwo_status = lampone_status = "OFF";
    }

  }
}
void lcd_display() {
  unsigned int current_millis = millis();
  if (current_millis - last_refresh > 2000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lamp 1: ");
    lcd.print(lampone_status);
    lcd.setCursor(0, 1);
    lcd.print("Lamp 2: ");
    lcd.print(lamptwo_status);
    last_refresh = millis();
  }

}
