#include <Servo.h>

int arm_raised = LOW;

int servo_pos = 0;    // Servo Position

// servo for the arm
Servo arm_servo;

void setup() {
  // put your setup code here, to run once:
  // Sets the servo and makes sure that the arm is raised
  arm_servo.attach(5); // Whatever pin the servo should be on !!!!!!!!!!!
  arm_servo.write(servo_pos); // Servo goes to 0
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
  servo_pos = 180;
  arm_servo.write(servo_pos);
  delay(1000);
  arm_raised = LOW;
}

void arm_raise(){
  // Raises the arm - we'll need to find the right position value
  servo_pos = 0;
  arm_servo.write(servo_pos);
  delay(1000);
  arm_raised = HIGH;
}
