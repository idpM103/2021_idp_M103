#include <Servo.h>
#include <Adafruit_MotorShield.h>

int arm_raised = LOW;
int reverse_state = LOW;
#define right_motor_speed 255     // Standard speed of motor
#define left_motor_speed 255

int servo_pos = 0;    // Servo Position

// servo for the arm
Servo arm_servo;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select motor ports
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(4);
Adafruit_DCMotor *RightMotor = AFMS.getMotor(3);

void setup() {
  // put your setup code here, to run once:
  // Sets the servo and makes sure that the arm is raised
  arm_servo.attach(5); // Whatever pin the servo should be on !!!!!!!!!!!
  arm_servo.write(servo_pos); // Servo goes to 0
  delay(1000); // Delay to make sure the servo goes to 0
  arm_raised = HIGH; // Sets arm flag
  AFMS.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  reverse_state = HIGH;
  forwards();
  delay(500); // We reverse a bit
  halt();
  reverse_state = LOW;

  rotate_left();
  delay(500); // We turn left a bit
  halt();

  // We drop the arm
  if (arm_raised == HIGH) {
    arm_drop();
  }

  rotate_right();
  delay(1000); // We turn left a lot (through the entire square), but not 180 yet
  halt();
  
 

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

void forwards() {
  LeftMotor->setSpeed(left_motor_speed);
  RightMotor->setSpeed(right_motor_speed);
  if (reverse_state == LOW) {
    LeftMotor->run(FORWARD);
    RightMotor->run(FORWARD);
  }
  else {
    LeftMotor->run(BACKWARD);
    RightMotor->run(BACKWARD);
  }
}


void halt() {
  Serial.println("stopping ");
  LeftMotor->run(RELEASE);
  RightMotor->run(RELEASE);
}

void rotate_right(){
  LeftMotor->setSpeed(left_motor_speed);
  RightMotor->setSpeed(right_motor_speed);
  LeftMotor->run(FORWARD);
  RightMotor->run(BACKWARD);
}

void rotate_left(){
  LeftMotor->setSpeed(left_motor_speed);
  RightMotor->setSpeed(right_motor_speed);
  LeftMotor->run(BACKWARD);
  RightMotor->run(FORWARD);
}
