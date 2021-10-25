/*
This is a test to make the robot move in a small square
For use with the Adafruit Motor Shield v2
---->	http://www.adafruit.com/products/1438
*/

#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Sets the left wheel motor to pin M3 and the Right to M4
Adafruit_DCMotor *Left_Wheel_Motor = AFMS.getMotor(4);
Adafruit_DCMotor *Right_Wheel_Motor = AFMS.getMotor(3);


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Rotation test!");
  
  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");
}

void loop() {

  
  Left_Wheel_Motor->setSpeed(200);
  Right_Wheel_Motor->setSpeed(215);
  Left_Wheel_Motor->run(BACKWARD);
  Right_Wheel_Motor->run(FORWARD);
  delay(5000);
  Serial.println("Test Complete");
  while (1);
  } 
