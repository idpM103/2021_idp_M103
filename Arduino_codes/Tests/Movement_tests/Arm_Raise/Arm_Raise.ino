#include <Servo.h>

int arm_raised = LOW;

int servo_pos = 0;    // Servo Position

// servo for the arm
Servo arm_servo;

void setup() {
  // put your setup code here, to run once:
  // Sets the servo and makes sure that the arm is raised
  delay(1000); // Delay to make sure the servo goes to 0
  arm_raised = HIGH; // Sets arm flag
}

void loop() {
  // put your main code here, to run repeatedly:

  // tests the rasing and lowering of the arm so we know what position to set it at.
  arm_drop();
  delay(2000);
  arm_raise();
  

}

void arm_drop(){
  // Drops the arm - we'll need to find the right position value
  arm_servo.attach(9);
  for (servo_pos = 180; servo_pos >= 90; servo_pos -= 1) { // goes from 180 degrees to 0 degrees
    arm_servo.write(servo_pos);              // tell servo to go to position in variable 'pos'
    delay(50);                       // waits 15 ms for the servo to reach the position
  }
  arm_servo.detach();
  arm_raised = LOW;
}

void arm_raise(){
  // Raises the arm - we'll need to find the right position value
  arm_servo.attach(9);
  for (servo_pos = 90; servo_pos <= 180; servo_pos += 1) { // goes from 180 degrees to 0 degrees
    arm_servo.write(servo_pos);              // tell servo to go to position in variable 'pos'
    delay(50);                       // waits 15 ms for the servo to reach the position
  }
  arm_servo.detach();
  arm_raised = HIGH;
}
