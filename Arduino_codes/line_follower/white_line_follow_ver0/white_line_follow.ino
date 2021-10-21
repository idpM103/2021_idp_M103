//'FORWARDS' IN CODE = 'FORWARD' ON AGV
//remember to set D10 high while driving, low when stopped (contorls amber LED)

#include <Wire.h>
#include <Adafruit_MotorShield.h>

#define LEFT_MOST A3
#define LEFT_INNER A2
#define RIGHT_INNER A1
#define RIGHT_MOST A0
const int buttonPin = 7;     // the number of the pushbutton pin

// variables will change:
int buttonState = HIGH;         // variable for reading the pushbutton status
int toggle_state = LOW;
int backward_state = LOW;

int left_most_value;      // Left line sensor value-  higher number => lighter
int left_inner_value;
int right_inner_value;     // Right line sensor value
int right_most_value;

int delta_threshold = 100;// Cut-off for sensor reading being over the line or not
int threshold = 900;
#define motor_speed 255     // Standard speed of motor
int both_option = 1;
int both_happening = 0;
int left0;
int left1;
int right0;
int right1;
int count = 0;
int turning_state = 0;

void forwards ();
void halt ();
void large_left ();
void small_left ();
void large_right ();
void small_right ();

unsigned long prev_increment_time_0 = 0;
unsigned long prev_increment_time_1 = 0;
const long min_increment_interval = 1000;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Select motor ports
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(3);
Adafruit_DCMotor *RightMotor = AFMS.getMotor(4);

void setup() {
  // Set up serial communication
  Serial.begin(9600);
  AFMS.begin();
  pinMode(buttonPin, INPUT);
}

void loop() {
  // Read line sensors and output values
  left_most_value  = analogRead(LEFT_MOST);
  left_inner_value  = analogRead(LEFT_INNER);
  right_inner_value = analogRead(RIGHT_INNER);
  right_most_value = analogRead(RIGHT_MOST);
  Serial.println(left_most_value);
  Serial.println(left_inner_value);
  Serial.println(left_inner_value);
  Serial.println(right_most_value);
  buttonState = digitalRead(buttonPin);

  // ultrasonic sensor 
  if (buttonState == LOW) {
    toggle_state = ! toggle_state; }
  

  if (toggle_state == LOW) {
  

      //delay(50);
  // if all black, keep moving forward
  
  if (left_most_value>threshold && left_inner_value>threshold && right_inner_value>threshold && right_most_value > threshold) {
    if (turning_state == 0){
    forwards();
    }
    else if(turning_state == 1){
      large_right();
    }
    else if(turning_state == 2){
      large_left();
    }
  }
  
  // if outskirt sensors white, stop
  
  else if (left_most_value<threshold && right_most_value < threshold) {
  
    halt();
    //delay(50);
    count += 1;

  }
  // a bit to the right, left most inner black,  white, right most black

  else if (left_most_value>threshold && left_inner_value>threshold && right_inner_value < threshold && right_most_value > threshold) {
    small_right();
    turning_state = 0;
    //delay(50);

    }
  // a LOT to the right, left most inner,right inner black, right most white  
   else if (left_most_value>threshold && left_inner_value>threshold && right_inner_value > threshold && right_most_value < threshold ) {
    large_right();
    turning_state = 1;
    //delay(100);

   
  }
    // a bit to the left
    else if (left_most_value>threshold && left_inner_value<threshold && right_inner_value > threshold && right_most_value > threshold) {
      small_left();
      turning_state = 0;
      //delay(50);

      }
    // a LOT to the left
    else if (left_most_value<threshold && left_inner_value>threshold && right_inner_value > threshold && right_most_value > threshold) {
      large_left();
      //delay(100);
      turning_state = 2;
      }

    if (count == 3){

      
      
      }
   

  }
  
 
  else {
  ;
    }
  }




void forwards(){
  LeftMotor->setSpeed(motor_speed); 
  RightMotor->setSpeed(motor_speed);
  LeftMotor->run(FORWARD);
  RightMotor->run(FORWARD);
}

void small_left(){
  Serial.println("turning right ");
  LeftMotor->setSpeed(motor_speed); 
  RightMotor->setSpeed(0.6*motor_speed);
  LeftMotor->run(FORWARD);
  RightMotor->run(FORWARD);
}

void large_left(){
  Serial.println("turning right ");
  LeftMotor->setSpeed(motor_speed); 
  RightMotor->setSpeed(0.05*motor_speed);
  LeftMotor->run(FORWARD);
  RightMotor->run(FORWARD);
}


void small_right(){
  Serial.println("turning left ");
  LeftMotor->setSpeed(0.6*motor_speed); 
  RightMotor->setSpeed(motor_speed);
  LeftMotor->run(FORWARD);
  RightMotor->run(FORWARD);
}

void large_right(){
  Serial.println("turning left ");
  LeftMotor->setSpeed(0.05*motor_speed); 
  RightMotor->setSpeed(motor_speed);
  LeftMotor->run(FORWARD);
  RightMotor->run(FORWARD);
}



void halt(){
  Serial.println("stopping ");
  LeftMotor->run(RELEASE);
  RightMotor->run(RELEASE);
}
/*
void both(int option){  //what to do when both sensors hit line. each option is in the sequence of what is expected to happen
  if (option == 1){ //option 1: Coming out of my cage
    forwards();
  }
  else if (option == 2){ //option 2: line merge from base to arena
    forwards();
  }
  else if (option == 3){ //option 3: stop at end of track
    halt();
  }
  //break for program in arean (ie not following lines)
  else if (option == 4){ //option 4: returning from arena to track
    forwards();
  }
  else if (option == 5){ //option 5: line merge from arena to service area
    right();
  }
  else if (option == 6){ //option 6: enter service area
    halt();  //AND DROP ROBOT
  }
  else if (option == 7){ //option 7: 180 degree turn
    //180 DEGREEE TURN, IGNORE FOR NOW
  }
  else if (option == 8){ //option 8: line merge from service area to arena
    left();
  }
  //here, return to option 3 for next robot. if final robot, stop before option 5, and do option 9 instead
  else if (option == 9){ //option 9: line merge from arena to base
    left();
  }
  else if (option == 10){ //option 10: going back into my cage
    halt();
    //ADJUST SO COMES INTO BASE FULLY
  }
}

void increment_option(){
  if ((both_happening == 1) && ((millis() - prev_increment_time_0) >= min_increment_interval)){
      both_happening = 0;
      both_option += 1;
      prev_increment_time_0 = millis();
    }
}
*/
