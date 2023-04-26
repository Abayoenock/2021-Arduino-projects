#include <IRremote.h>
const int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;
String pin1 = "465573243";//1
String pin2 = "16714485";//2
String pin3 = "1162296347";//3
String pin4 = "2388475579";//4
String clear_key = "961851831";//clear key
int receive_check = 0;// the variable to increament the pins from the receiver
String received_keys = "";
String received_keys1 = "";
String received_keys2 = "";
String received_keys3 = "";
String received_keys4 = "";
int motor_forward = 4;
int motor_reverse = 5;
int limit_forward = 6;
int limit_reverse = 7;
void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  pinMode(motor_forward, OUTPUT);
  pinMode(motor_reverse, OUTPUT);
  pinMode(limit_forward, INPUT);
  pinMode(limit_reverse, INPUT);
  digitalWrite(motor_forward, HIGH);
  digitalWrite(motor_reverse, HIGH);

}

void loop() {
  if (irrecv.decode(&results)) {
    received_keys = results.value;
    if (received_keys == "4294967295" || received_keys == "255") {
     
    }
    else {
      if (received_keys == clear_key) {
        receive_check = 0;
      }
      else if (received_keys == "3691091931") {
        close_gate();
      }
      else {
        if (receive_check == 0) {
          received_keys1 = results.value;
          receive_check++;
        }
        if (receive_check == 1) {
          received_keys2 = results.value;
          receive_check++;
        }
        if (receive_check == 2) {
          received_keys3 = results.value;
          receive_check++;
        }
        if (receive_check == 3) {
          received_keys4 = results.value;
          check_if();// to check if the pinis correct
          receive_check = 0;

        }



        Serial.println(received_keys);
        irrecv.resume();
      }
    }
  }
}
void check_if() {// the function to check if the password is correct
  if (received_keys1 == pin1 && received_keys2 == pin2 && received_keys3 == pin3 && received_keys4 == pin4) {
    open_gate();// the fuction to open the gate when the pasword is correct
  }
}
void open_gate() {// open gate fuction
 
  while (1) { // loop to llop until the gate touches the limit switch
    if (digitalRead(limit_reverse == HIGH)) {
      stop_f();// the fuction to stop the motor when the limit switch is pressed
      break;
    }
    else {
      digitalWrite(motor_reverse, LOW);
      digitalWrite(motor_forward, HIGH);
    }
    
  }
  Serial.println("opening gate");

}
void stop_f() {// the function to stop the motor
  digitalWrite(motor_reverse, HIGH);
  digitalWrite(motor_forward, HIGH);
}
void close_gate() {// the function to close the gate
  while (1) {
    if (digitalRead(limit_forward == HIGH)) {
      break;
      stop_f();
    }
    else {
      digitalWrite(motor_reverse, HIGH);
      digitalWrite(motor_forward, LOW);
    }
   
  }
  Serial.println("cloing gate ");
}
