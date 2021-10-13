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
Adafruit_DCMotor *Left_Wheel_Motor = AFMS.getMotor(3);
Adafruit_DCMotor *Right_Wheel_Motor = AFMS.getMotor(4);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Square Movement Test!");

  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");
}

void loop() {
  uint8_t i;

  Serial.print("Start");
  
  for(int i=0; i <=3; i++){
    Left_Wheel_Motor->setSpeed(100);
    Right_Wheel_Motor->setSpeed(100);
    Left_Wheel_Motor->run(FORWARD);
    Right_Wheel_Motor->run(FORWARD);
    delay(10000);
    Right_Wheel_Motor->run(RELEASE);
    delay(10000);
    Serial.print("Side Complete");
    
  }
  Left_Wheel_Motor->run(RELEASE);
  return;
}
