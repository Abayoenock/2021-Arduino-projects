int motor_right_a = 2;
int motor_right_b = 3;
int motor_left_a = 5;
int motor_left_b = 4;
int L_S = A3;
int S_S = A2;
int R_S = A1;
int speed_motor_a = 9;
int speed_motor_b = 10;
int trigPin = 11; // Trigger
int echoPin = 12; // Echo
int speed_definer = 6;
long duration, cm, inches;
int lift_motor_a = A5;
int lift_motor_b = A0;
int stop_down = 7;
int stop_up = 8;
int lifted = 0;
void setup()
{
  Serial.begin(9600);
  pinMode(motor_right_a, OUTPUT);
  pinMode(motor_left_a, OUTPUT);
  pinMode(motor_right_b, OUTPUT);
  pinMode(motor_left_b, OUTPUT);
  pinMode(lift_motor_a, OUTPUT);
  pinMode(lift_motor_b, OUTPUT);
  pinMode(stop_down, INPUT);
  pinMode(stop_up, INPUT);
  pinMode(speed_motor_a, OUTPUT);
  pinMode(speed_motor_b, OUTPUT);
  pinMode(speed_definer, INPUT);
  pinMode(L_S, INPUT);
  pinMode(S_S, INPUT);
  pinMode(R_S, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(lift_motor_a, 0);
  digitalWrite(lift_motor_b, 0);
  up_lift();
}
void loop()
{
  if (digitalRead(speed_definer) == HIGH) {
    analogWrite(speed_motor_a, 130);
    analogWrite(speed_motor_b, 130);
  }
  else {
    analogWrite(speed_motor_a, 100);
    analogWrite(speed_motor_b, 100);
  }


  obstacle();
  if (cm < 10) {
    Stop();
  }
  if ((digitalRead(L_S) == 0) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 0)) {
      forword();
    
  }
  if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 0) && lifted == 1) {
    Stop();
    delay(1000);
    up_in_place();
  }
  if ((digitalRead(L_S) == 0) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 1) && lifted == 1 ) {
    Stop();
    delay(1000);
    up_in_place();
  }
  if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 0)) {
    turnLeft();
  }
  if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 0) && (digitalRead(R_S) == 0)) {
    turnLeft();
  }

  if ((digitalRead(L_S) == 0) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 1)) {
    turnRight();
  }
  if ((digitalRead(L_S) == 0) && (digitalRead(S_S) == 0) && (digitalRead(R_S) == 1)) {
    turnRight();
  }


  if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 1)) { // when the car reaches the stop place
    Stop();
    delay(2000);
    if (cm < 40) {
      down_lift();
      while (1) {
        analogWrite(speed_motor_a, 130);
        analogWrite(speed_motor_b, 130);
        obstacle();
        if (cm < 6) {
          Stop();
          delay(1000);
          up_lift();
          delay(1000);
          lifted = 1;
          forword();
          delay(500);
          break;
        }
        else {
          forword();
        }
      }
    }
  }

}

void forword() {

  digitalWrite(motor_left_a, HIGH);
  digitalWrite(motor_left_b, LOW);
  digitalWrite(motor_right_a, HIGH);
  digitalWrite(motor_right_b, LOW);
}
void backward() {
  digitalWrite(motor_left_a, LOW);
  digitalWrite(motor_left_b, HIGH);
  digitalWrite(motor_right_a, LOW);
  digitalWrite(motor_right_b, HIGH);
}

void turnRight() {
 
  digitalWrite(motor_left_a, HIGH);
  digitalWrite(motor_left_b, LOW);
  digitalWrite(motor_right_a, LOW);
  digitalWrite(motor_right_b, LOW);


}

void turnLeft() {
 
  digitalWrite(motor_left_a, LOW);
  digitalWrite(motor_left_b, LOW);
  digitalWrite(motor_right_a, HIGH);
  digitalWrite(motor_right_b, LOW);
}

void Stop() {
  digitalWrite(motor_left_a, LOW);
  digitalWrite(motor_left_b, LOW);
  digitalWrite(motor_right_a, LOW);
  digitalWrite(motor_right_b, LOW);
}
void obstacle()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  // Convert the time into a distance
  cm = (duration / 2) / 29.1; // Divide by 29.1 or multiply by 0.0343
  Serial.println(cm);
}
void down_lift() {

  while (1) {
    if (digitalRead(stop_down) == HIGH) {
      digitalWrite(lift_motor_a, LOW);
      digitalWrite(lift_motor_b, LOW);
      break;
    }
    else {

      digitalWrite(lift_motor_a, 255);
      digitalWrite(lift_motor_b, 0);
    }

  }

}
void up_lift() {

  while (1) {
    if (digitalRead(stop_up) == HIGH) {
      digitalWrite(lift_motor_a, 0);
      digitalWrite(lift_motor_b, 0);
      break;
    }
    else {

      digitalWrite(lift_motor_a, 0);
      digitalWrite(lift_motor_b, 255);
    }
  }
}
void up_in_place() {
  while (1) {
    analogWrite(speed_motor_a, 100);
    analogWrite(speed_motor_b, 100);

    if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 1)) {
      Stop();
      delay(2000);
      down_lift();
      delay(2000);
      back_line();
      break;
    }
    else {
      forword();
    }
  }

}
void back_line() {
  int x = 0;
  while (1) {
    analogWrite(speed_motor_a, 110);
    analogWrite(speed_motor_b, 110);
    if (x == 0) {
      backward();
      delay(1000);
      x++;
    }
    if ((digitalRead(L_S) == 1) || (digitalRead(S_S) == 1) || (digitalRead(R_S) == 1)) {
      Stop();
      delay(2000);
      up_lift();
      lifted = 0;
      delay(2000);
      break;
    }
    else {
      backward();
    }
  }
}
