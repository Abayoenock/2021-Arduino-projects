#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27
static const int RXPin_gsm = 10, TXPin_gsm = 11;//GSM
SoftwareSerial gsm(RXPin_gsm, TXPin_gsm);
int meter_id = 9889;// to hold the meter unique identifier
String my_web = "http://s-meter.enlabs.rw/api.php"; // the link to the web to fetch the data
String pin = ""; // to hold the meter password
String input_password;// to hold the pin as the user types
float meter_balance = 0; // to hold the meter balance
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns
String phone_num = ""; // to hold the phone number
int valve_status = 1; // the variable to check if not remotely locked
int valve_trigger = 12;// the pin connected to the relay to trigger the selonid valve
int steps = 0;// to continusly check the next step to send the data via the gsm
int s = 0;// to initialize the send of at commands
unsigned long delayer_millis = 0;// the amount of delay before next command
unsigned long currentMillis_d = 0;// to holld the current time
unsigned long lastExecutedMillis_d = 0;// to hold the last miilisenconds that the data was sent
unsigned long lastExecutedMillis_d_d = 0;// to hold the last miilisenconds that the display was updated
unsigned long lastExecutedMillis_d_l = 0;// to hold the last miilisenconds for the auto lock
int check_auto_lock = 0; // to continously check if there is no flow and lock the elactro valve
int check_auto_lock_d = 0; // to continously check if there is no flow and lock the elactro valve
volatile int flow_frequency; // Measures flow sensor pulses
// Calculated litres/hour
float vol = 0.0, l_minute;
unsigned char flowsensor = 2; // Sensor Input for the water flow sensor
unsigned long currentTime;
unsigned long cloopTime;
float balance_m = 0;// to hold the balance of the meter
int keyz = 0;// to check if keyz are pressed
String hidden_pin = ""; // to diplay * on pin input
int  message_delayer = 0;// to display the content with extended delay
int em = 0; // to send notification emails
int  email_not = 1; // to test if notification was sent
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3 }; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
int red = A0;
int green = A1;
int blue = A2;
void setup()
{
  Serial.begin(115200);
  gsm.begin(9600);
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart water");
  lcd.setCursor(0, 1);
  lcd.print("meter");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("connecting ");
  lcd.setCursor(0, 1);
  lcd.print("to server ");
  delay(5000);
  password_balance();// to retrive the password and the balance in the meter
  phone_status();// to retrive the phonen number and the status of the valve
  pinMode(valve_trigger, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  digitalWrite(valve_trigger, HIGH);
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
  attachInterrupt(digitalPinToInterrupt(flowsensor), flow, RISING); // Setup Interrupt
  currentTime = millis();
  cloopTime = currentTime;

}
void flow () // Interrupt function
{
  flow_frequency++;
}
void loop()
{


  print_screen();
  pins();
  used_water();
  reds();
  send_data();


}


void password_balance() {
  gsm.println("AT+CSQ"); // Signal quality check
  delay(100);
  ShowData();
  gsm.println("AT+CGATT?"); //Attach or Detach from GPRS Support
  delay(100);
  ShowData();
  gsm.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");//setting the SAPBR, the connection type is using gprs
  delay(200);
  ShowData();
  gsm.println("AT+SAPBR=3,1,\"APN\",\"INTERNET\"");//setting the APN, Access point name string
  delay(1000);
  ShowData();
  gsm.println("AT+SAPBR=1,1");//setting the SAPBR
  delay(200);
  ShowData();
  gsm.println("AT+HTTPINIT"); //init the HTTP request
  delay(200);
  ShowData();
  String task = "info";
  String url = my_web + "?t=" + task + "&m=" + meter_id;
  gsm.println("AT+HTTPPARA=\"URL\"," + url);
  delay(1000);
  ShowData();
  gsm.println("AT+HTTPACTION=0");//submit the request
  delay(4000);
  ShowData();
  gsm.println("AT+HTTPREAD");// read the data from the website you access
  delay(9000);
  get_content();
  delay(1000);
}
void phone_status() {
  gsm.println("AT+CSQ"); // Signal quality check
  delay(100);
  ShowData();
  gsm.println("AT+CGATT?"); //Attach or Detach from GPRS Support
  delay(100);
  ShowData();
  gsm.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");//setting the SAPBR, the connection type is using gprs
  delay(200);
  ShowData();
  gsm.println("AT+SAPBR=3,1,\"APN\",\"INTERNET\"");//setting the APN, Access point name string
  delay(1000);
  ShowData();
  gsm.println("AT+SAPBR=1,1");//setting the SAPBR
  delay(200);
  ShowData();
  gsm.println("AT+HTTPINIT"); //init the HTTP request
  delay(200);
  ShowData();
  String task = "4ne";
  String url = my_web + "?t=" + task + "&m=" + meter_id;
  gsm.println("AT+HTTPPARA=\"URL\"," + url);
  delay(1000);
  ShowData();
  gsm.println("AT+HTTPACTION=0");//submit the request
  delay(4000);
  ShowData();
  gsm.println("AT+HTTPREAD");// read the data from the website you access
  delay(9000);
  get_phone_status();
  delay(1000);
}

void ShowData()
{
  while (gsm.available() != 0) {
    Serial.write(char (gsm.read()));
  }
}
void get_content()
{
  String vals = "";
  while (gsm.available() != 0)
  {
    vals = vals + (char (gsm.read()));
  }

  vals = vals.substring(29, 80);
  int leng = vals.length();
  int rem = leng - 6;
  vals.remove(rem, leng);
  Serial.println(vals);
  String valll = vals;
  const char* input = valll.c_str();
  delay(100);
  StaticJsonDocument<32> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
//    lcd.clear();
//    lcd.setCursor(0, 0);
//    lcd.print("Reconnecting ");
//    lcd.setCursor(0, 1);
//    lcd.print("to server ");
//    password_balance();// to retrive the password and the balance in the meter
    return;
  }
  String  password = doc["p"];// to hold the meter password
  meter_balance = doc["b"];// to hold the balance retrieved from the web
  pin = password;
  balance_m = meter_balance;
  Serial.println(password);
  Serial.println(meter_balance);


  delay(1000);
}

void get_phone_status()
{
  String vals = "";
  while (gsm.available() != 0)
  {
    vals = vals + (char (gsm.read()));
  }
  vals = vals.substring(29, 80);
  Serial.print(vals);
  int leng = vals.length();
  int rem = leng - 6;
  vals.remove(rem, leng);
  Serial.println(vals);
  String valll = vals;
  const char* input = valll.c_str();
  StaticJsonDocument<48> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
//    lcd.clear();
//    lcd.setCursor(0, 0);
//    lcd.print("Reconnecting ");
//    lcd.setCursor(0, 1);
//    lcd.print("to server ");
//    phone_status();// to retrive the phonen number and the status of the valve
    return;
  }
  String  p = doc["p"];
  valve_status = doc["s"];
  phone_num = p;
  if (valve_status == 1) {
    digitalWrite(valve_trigger, HIGH);
  }
  Serial.println(phone_num);
  Serial.println(valve_status);

}
void get_status_responce()
{
  String vals = "";
  while (gsm.available() != 0)
  {
    vals = vals + (char (gsm.read()));
  }
  vals = vals.substring(29, 80);
  int leng = vals.length();
  int rem = leng - 6;
  vals.remove(rem, leng);
  Serial.println(vals);
  String valll = vals;
  const char* input = valll.c_str();
  StaticJsonDocument<32> doc;
  DeserializationError err = deserializeJson(doc, input) ;
  if (err) {
    Serial.print("ERROR:");
    Serial.print(err.c_str());
    return;
  }
  int   sent_status = doc["d"];
  valve_status = doc["s"];
  if (valve_status == 1) {
    digitalWrite(valve_trigger, HIGH);
  }
}


void pins() {
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);
    keyz = 1;
    if (key == '*') {
      keyz = 0;
      hidden_pin = "";
      input_password = ""; // clear input password
    } else if (key == '#') {
      if (pin == input_password) {
        Serial.println("password is correct");
        if (valve_status == 0) {

          message_delayer = 1;
          if (balance_m <= 0) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("You ran out");
            lcd.setCursor(0, 1);
            lcd.print("of water !");
          }
          else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Successful");
            lcd.setCursor(0, 1);
            lcd.print("Open the tap");
            digitalWrite(valve_trigger, LOW);
            check_auto_lock_d = 0;
            check_auto_lock = 0;
          }


        }
        else {
          message_delayer = 1;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Remotely");
          lcd.setCursor(0, 1);
          lcd.print("locked !");


        }




      } else {
        message_delayer = 1;
        Serial.println("password is incorrect, try again");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Incorrect pin");
        lcd.setCursor(0, 1);
        lcd.print("Try again");

      }
      hidden_pin = "";
      keyz = 0;
      input_password = ""; // clear input password
    } else {
      input_password += key; // append new character to input password string
      hidden_pin += "*";

    }
  }
}

void send_data() {

  currentMillis_d = millis();

  if (steps == 0) {
    if (s == 0) {
      s += 1;
      gsm.println("AT+CSQ"); // Signal quality check

    }
    delayer_millis = 100;
    if (currentMillis_d - lastExecutedMillis_d >= delayer_millis) {
      lastExecutedMillis_d = currentMillis_d; //
      ShowData();
      steps = 1;
      s = 0;
    }
  }

  if (steps == 1) {
    if (s == 0) {
      s += 1;
      gsm.println("AT+CGATT?"); //Attach or Detach from GPRS Support
    }
    delayer_millis = 100;
    if (currentMillis_d - lastExecutedMillis_d >= delayer_millis) {
      lastExecutedMillis_d = currentMillis_d; //
      ShowData();
      steps = 2;
      s = 0;
    }
  }

  if (steps == 2) {
    if (s == 0) {
      s += 1;
      gsm.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");//setting the SAPBR, the connection type is using gprs
    }
    delayer_millis = 200;
    if (currentMillis_d - lastExecutedMillis_d >= delayer_millis) {
      lastExecutedMillis_d = currentMillis_d; //
      ShowData();
      steps = 3;
      s = 0;
    }
  }

  if (steps == 3) {
    if (s == 0) {
      s += 1;
      gsm.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");//setting the SAPBR, the connection type is using gprs
    }
    delayer_millis = 200;
    if (currentMillis_d - lastExecutedMillis_d >= delayer_millis) {
      lastExecutedMillis_d = currentMillis_d; //
      ShowData();
      steps = 4;
      s = 0;
    }
  }
  if (steps == 4) {
    if (s == 0) {
      s += 1;
      gsm.println("AT+SAPBR=3,1,\"APN\",\"INTERNET\"");//setting the APN, Access point name string
    }
    delayer_millis = 1000;
    if (currentMillis_d - lastExecutedMillis_d >= delayer_millis) {
      lastExecutedMillis_d = currentMillis_d; //
      ShowData();
      steps = 5;
      s = 0;
    }
  }
  if (steps == 5) {
    if (s == 0) {
      s += 1;
      gsm.println("AT+SAPBR=1,1");//setting the SAPBR
    }
    delayer_millis = 200;
    if (currentMillis_d - lastExecutedMillis_d >= delayer_millis) {
      lastExecutedMillis_d = currentMillis_d; //
      ShowData();
      steps = 6;
      s = 0;
    }
  }
  if (steps == 6) {
    if (s == 0) {
      s += 1;
      gsm.println("AT+HTTPINIT"); //init the HTTP request
    }
    delayer_millis = 200;
    if (currentMillis_d - lastExecutedMillis_d >= delayer_millis) {
      lastExecutedMillis_d = currentMillis_d; //
      ShowData();
      steps = 7;
      s = 0;
    }
  }
  if (steps == 7) {

    if (balance_m < 230 && email_not == 0) {
      em = 1;
      email_not = 1;
    }
    String task = "d";
    String url = my_web + "?t=" + task + "&m=" + meter_id + "&b=" + balance_m + "&em=" + em;
    if (s == 0) {
      s += 1;
      gsm.println("AT+HTTPPARA=\"URL\"," + url);
    }
    delayer_millis = 1000;
    if (currentMillis_d - lastExecutedMillis_d >= delayer_millis) {
      lastExecutedMillis_d = currentMillis_d; //
      ShowData();
      steps = 8;
      s = 0;
    }
  }
  if (steps == 8) {
    if (s == 0) {
      s += 1;
      gsm.println("AT+HTTPACTION=0");//submit the request
    }
    delayer_millis = 4000;
    if (currentMillis_d - lastExecutedMillis_d >= delayer_millis) {
      lastExecutedMillis_d = currentMillis_d; //
      ShowData();
      steps = 9;
      s = 0;
    }
  }
  if (steps == 9) {
    if (s == 0) {
      s += 1;
      gsm.println("AT+HTTPREAD");// read the data from the website you access
    }
    delayer_millis = 9000;
    if (currentMillis_d - lastExecutedMillis_d >= delayer_millis) {
      lastExecutedMillis_d = currentMillis_d; //

      get_status_responce();
      steps = 0;
      s = 0;
    }
  }

}
void used_water()
{
  currentTime = millis();
  // Every second, calculate and print litres/hour
  if (currentTime >= (cloopTime + 1000))
  {
    cloopTime = currentTime; // Updates cloopTime
    if (flow_frequency != 0) {
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_minute = (flow_frequency / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      Serial.print("Rate: ");
      Serial.print(l_minute);
      Serial.println(" L/M");
      l_minute = l_minute / 60;

      vol = vol + l_minute;
      Serial.println("Vol:");
      Serial.print(vol);
      Serial.print(" L");
      flow_frequency = 0; // Reset Counter
      Serial.print(l_minute, DEC); // Print litres/hour
      Serial.println(" L/Sec");
      check_auto_lock = 0;
      check_auto_lock_d = 0;
    }
    else {
      if (check_auto_lock == 0) {
        lastExecutedMillis_d_l = millis();
        check_auto_lock += 1;
      }
      Serial.println(" flow rate = 0 ");

      Serial.print("Rate: ");
      Serial.print( flow_frequency );
      Serial.print(" L/M");

      Serial.println("Vol:");
      Serial.print(vol);
      Serial.print(" L");
      currentMillis_d = millis();
      delayer_millis = 30000;
      if (currentMillis_d - lastExecutedMillis_d_l >= delayer_millis) {
        if ( check_auto_lock_d == 0) {
          message_delayer = 1;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Auto lock");
          lcd.setCursor(0, 1);
          lcd.print("initiated....");
          digitalWrite(valve_trigger, HIGH);
          check_auto_lock_d += 1;
        }
      }







    }
    balance_m = meter_balance - vol;
    if (balance_m <= 0) {
      digitalWrite(valve_trigger, HIGH);
    }
  }
}
void print_screen() {
  currentMillis_d = millis();
  if ( message_delayer == 1) {
    delayer_millis = 5000;

  } else {
    if (keyz == 1) {
      delayer_millis = 500;
    }
    else {
      delayer_millis = 1000;
    }
  }


  if (currentMillis_d - lastExecutedMillis_d_d >= delayer_millis) {
    lastExecutedMillis_d_d = currentMillis_d; //
    if (keyz == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PIN: ");
      lcd.print(hidden_pin);

    }
    else {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bal: ");
      lcd.print((balance_m/1000));
      lcd.print("M3");
      lcd.setCursor(0, 1);
      lcd.print(vol);
       lcd.print(" L");
      message_delayer = 0;
    }
  }

}
void reds() {
  if (balance_m > 300) {
    analogWrite(red, 0);
    analogWrite(green, 255);
    analogWrite(blue, 0);

  }
  else if (balance_m < 300 && balance_m > 100) {
    analogWrite(red, 0);
    analogWrite(green, 255);
    analogWrite(blue, 200);
  }
  else {
    analogWrite(red, 255);
    analogWrite(green, 0);
    analogWrite(blue, 0);

  }
}
