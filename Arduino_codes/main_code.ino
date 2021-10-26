//'FORWARDS' IN CODE = 'FORWARD' ON AGV
//remember to set D10 high while driving, low when stopped (contorls amber LED)

#include <Wire.h>
#include <Adafruit_MotorShield.h>

//Pins
#define left_most A3
#define left_inner A2
#define right_inner A1
#define right_most A0
const int buttonPin = 7;    // the number of the pushbutton pin


// Flags
int previous_button_value = LOW;
int current_button_value = LOW;
int button_state = LOW;        
int reverse_state = LOW;
int line_follow_state = HIGH;
int block_detect_state = LOW;

//Other variables
int button_counter = 0;
int junction_counter = 0;
int left_most_value;      // Left line sensor value-  higher number => lighter
int left_inner_value;
int right_inner_value;     // Right line sensor value
int right_most_value;
int delta_threshold = 100;// Cut-off for sensor reading being over the line or not
int left_threshold = 120;
int right_threshold = 600;
#define right_motor_speed 200     // Standard speed of motor
#define left_motor_speed 255


// Function declarations
void forwards ();
void halt ();
void left ();
void right ();
void rotate_right();
void rotate_left();
void reset_wibich();

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select motor ports
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(4);
Adafruit_DCMotor *RightMotor = AFMS.getMotor(3);

void setup() {
  // Set up serial communication
  Serial.begin(9600);
  AFMS.begin();
  pinMode(buttonPin, INPUT);
}
//确认button按下输出低
void loop() {
  // Use a button to start and end the operation of the robot.
  previous_button_value = current_button_value;
  current_button_value = digitalRead(buttonPin);
  if (previous_button_value == HIGH && current_button_value == LOW){
      button_state = !button_state;
      previous_button_value = current_button_value;
  }
  else if (previous_button_value == LOW && current_button_value == HIGH){
      previous_button_value = current_button_value;
  }
  
  
  //The operation of the robot will only be triggered if the button state is true.
  if (button_state == HIGH) {
    // Read line sensors and output values
    left_most_value  = analogRead(left_most);
    left_inner_value  = analogRead(left_inner);
    right_inner_value = analogRead(right_inner);
    right_most_value = analogRead(right_most);
    /*
    Serial.print("leftmost:");
    Serial.println(left_most_value);
    Serial.print("leftinner:");
    Serial.println(left_inner_value);
    */
    Serial.print("rightinner:");
    Serial.println(right_inner_value);
    Serial.print("rightmost:");
    Serial.println(right_most_value);
    
    //Below is a counter for the number of junctions encountered.
    if (junction_counter >= 2) {
      reverse_state = !reverse_state;
      line_follow_state = HIGH;
      block_detect_state = LOW;
      junction_counter = 0;
    }


    
    // Below is the block detection block
    if(block_detect_state == HIGH){
      
    }
    // End of the block detection block


    
    //Below is the line following block. Adjust the delay to change the degree of turning.
    //Line following function(to be executed when the line follow flag is on)
    if(line_follow_state == HIGH){
    if (left_most_value > left_threshold && left_inner_value > left_threshold && right_inner_value > right_threshold && right_most_value > right_threshold) {
      forwards();
    }
    // On the junction
    else if (left_most_value < left_threshold && left_inner_value < left_threshold && right_inner_value < right_threshold && right_most_value < right_threshold) {
      //halt();
      junction_counter++;
      forwards();
      delay(1000);
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
    // End of the line following block.

    
  }
}






// Below are the motor functions.
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

void right() {
  Serial.println("turning right ");
  LeftMotor->setSpeed(left_motor_speed);
  if (reverse_state == LOW) {
    RightMotor->setSpeed(right_motor_speed);
    LeftMotor->run(FORWARD);
    RightMotor->run(RELEASE);
  }
  else {
    rotate_right();
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


void reset_wibich(){
  halt();
  button_state = LOW;        
  reverse_state = HIGH;
  line_follow_state = HIGH;
  block_detect_state = LOW;
  button_counter = 0;
  junction_counter = 0;
}
