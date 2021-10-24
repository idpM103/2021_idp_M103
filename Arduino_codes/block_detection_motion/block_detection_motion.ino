#include <NewPing.h> 
#include <Wire.h>
#include <Adafruit_MotorShield.h>

const int front_ultrasonic_trigPin = 8;
const int front_ultrasonic_echoPin = 9;
const int side_ultrasonic_trigPin = 10;
const int side_ultrasonic_echoPin = 11;
int duration, distance; // Add types 'duration' and 'distance'.

void detect();
void line_follow();
void forwards ();
void halt ();
void left ();
void right ();
void rotate_right();
void rotate_left();

int block_detect_state = HIGH;
int block_find_state = LOW;
int corner_state = LOW;
int reverse_state = LOW;
int left_most_value;      // Left line sensor value-  higher number => lighter
int left_inner_value;
int right_inner_value;     // Right line sensor value
int right_most_value;
int delta_threshold = 100;// Cut-off for sensor reading being over the line or not
int left_threshold = 120;
int right_threshold = 600;
#define right_motor_speed 200     // Standard speed of motor
#define left_motor_speed 255

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select motor ports
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(4);
Adafruit_DCMotor *RightMotor = AFMS.getMotor(3);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  AFMS.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  if (block_detect_state == HIGH){
    detect();
  } else{
    line_follow();
  }

}

void detect(void){
  if (block_find_state == LOW){
    rotate_left();
    line_follow();
    digitalWrite (side_ultrasonic_trigPin, HIGH);
    delay(50);
    digitalWrite (side_ultrasonic_trigPin, LOW);
    duration=pulseIn(side_ultrasonic_echoPin,HIGH);
    distance=(duration/2)/29.1;
    //at the corner 
    //if (left_most_value > left_threshold && left_inner_value < left_threshold && right_inner_value < right_threshold && right_most_value < right_threshold){
    // rotate_right();
    // line_follow();
      if (distance < 35){
        block_find_state = HIGH;
      } else {
        block_find_state = LOW;
      } 
    //}
    //if (left_most_value < left_threshold && left_inner_value < left_threshold && right_inner_value < right_threshold && right_most_value < right_threshold){
    // halt();
    //back to junction, waiting for further commands
  //}

  }
}


  



void left() {
  Serial.println("turning left ");
  LeftMotor->setSpeed(left_motor_speed);
  RightMotor->setSpeed(right_motor_speed);
  if (reverse_state == LOW) {
    LeftMotor->run(RELEASE);
    RightMotor->run(FORWARD);
  }
  else {
    rotate_left();
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


// Line follow function. Simply follow a straight line.
void line_follow(void){
    if (left_most_value > left_threshold && left_inner_value > left_threshold && right_inner_value > right_threshold && right_most_value > right_threshold) {
      forwards();
    }
    //Right inner sensor on the line (Small right turn)
    else if (left_most_value > left_threshold && left_inner_value > left_threshold && right_inner_value < right_threshold && right_most_value > right_threshold ) {
      if (reverse_state == LOW){
        right();
        delay(50);
      }
    }
    //Left inner sensor on the line  (Small left turn)
    else if (left_most_value > left_threshold && left_inner_value < left_threshold && right_inner_value > right_threshold && right_most_value > right_threshold ) {
      if (reverse_state == LOW){
        left();
      }
      //delay(50);
    }
    //Right outer sensor on the line (Large right turn)
    else if (left_most_value > left_threshold && left_inner_value > left_threshold && right_inner_value > right_threshold && right_most_value < right_threshold) {
      right();
      delay(250);
    }
    //Left outer sensor on the line (Large left turn)
    else if (left_most_value < left_threshold && left_inner_value > left_threshold && right_inner_value > right_threshold && right_most_value > right_threshold) {
      left();
      //delay(250);
    }
}
