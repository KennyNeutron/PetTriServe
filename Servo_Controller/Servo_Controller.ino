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

  // Initial position
  myServo1.write(90);
  myServo2.write(90);
  myServo3.write(90);

  Serial.println("Servo Controller Ready");
  Serial.println("Enter command (e.g., S1: 120)");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // Remove any leading/trailing whitespace

    if (command.length() > 0) {
      int separatorIndex = command.indexOf(':');

      if (separatorIndex != -1) {
        String servoPart = command.substring(0, separatorIndex);
        String anglePart = command.substring(separatorIndex + 1);

        servoPart.trim();
        anglePart.trim();

        // Check format "S1", "S2", "S3"
        if ((servoPart.startsWith("S") || servoPart.startsWith("s")) && servoPart.length() > 1) {
          int servoNum = servoPart.substring(1).toInt();
          int angle = anglePart.toInt();

          // Validate Servo Number
          if (servoNum >= 1 && servoNum <= 3) {
            // Validate Angle (90 to 180)
            if (angle >= 90 && angle <= 180) {
              switch (servoNum) {
                case 1:
                  myServo1.write(angle);
                  Serial.print("Servo 1 moved to ");
                  break;
                case 2:
                  myServo2.write(angle);
                  Serial.print("Servo 2 moved to ");
                  break;
                case 3:
                  myServo3.write(angle);
                  Serial.print("Servo 3 moved to ");
                  break;
              }
              Serial.println(angle);
            } else {
              Serial.println("Error: Angle must be between 90 and 180.");
            }
          } else {
            Serial.println("Error: Servo number must be 1, 2, or 3.");
          }
        } else {
          Serial.println("Error: Invalid command format. Use S1: 120");
        }
      }
    }
  }
}
