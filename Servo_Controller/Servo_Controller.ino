#include <Servo.h>

Servo myServo1;  // create servo object
Servo myServo2;  // create servo object
Servo myServo3;  // create servo object

int servoPin1 = 5;  // pin connected to servo signal wire
int servoPin2 = 6;  // pin connected to servo signal wire
int servoPin3 = 9;  // pin connected to servo signal wire

void setup() {
  Serial.begin(115200);  // Initialize serial communication

  myServo1.attach(servoPin1);  // attach servo to pin 5
  myServo2.attach(servoPin2);  // attach servo to pin 6
  myServo3.attach(servoPin3);  // attach servo to pin 9


  delay(1000);
  myServo1.write(90);
  delay(3000);
  myServo1.write(180);
  delay(1000);
  myServo2.write(90);
  delay(3000);
  myServo2.write(180);
  delay(1000);
  myServo3.write(90);
  delay(3000);
  myServo3.write(180);
 }

void loop() {
}
