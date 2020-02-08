#include <Servo.h>

Servo myservo;

void setup() {
  // put your setup code here, to run once:
  myservo.attach(9);
}

void loop() {
  // put your main code here, to run repeatedly:
  int value = map(analogRead(A0), 0, 1025, 0, 180);
  myservo.write(value);
}
