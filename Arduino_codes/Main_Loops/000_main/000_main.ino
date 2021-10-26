//'FORWARDS' IN CODE = 'FORWARD' ON AGV
//remember to set D10 high while driving, low when stopped (contorls amber LED)

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <Servo.h>

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
int block_detect_state = LOW; // This one is to start the detection/collection process
int block_detected_state = LOW; // This one sees if there is a block there
int end_of_rotation = LOW;
int rotate_left_flag = LOW;
int arm_raised = LOW;

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
int servo_pos = 0;
#define right_motor_speed 255     // Standard speed of motor
#define left_motor_speed 255


// Function declarations
void line_follow();
void forwards ();
void halt ();
void left ();
void right ();
void rotate_right();
void rotate_left();
unsigned long prev_increment_time_0 = 0;
unsigned long prev_increment_time_1 = 0;
const long min_increment_interval = 1000;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select motor ports
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(4);
Adafruit_DCMotor *RightMotor = AFMS.getMotor(3);

// servo for the arm
Servo arm_servo;

void setup() {
  // Set up serial communication
  Serial.begin(9600);
  AFMS.begin();

  pinMode(buttonPin, INPUT);

  // Sets the servo and makes sure that the arm is raised
  arm_servo.attach(5); // Whatever pin the servo should be on !!!!!!!!!!!
  arm_servo.write(servo_pos); // Servo goes to 0
  delay(1000); // Delay to make sure the servo goes to 0
  arm_raised = HIGH; // Sets arm flag
  
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

  // The main block of the operation of the robot.
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
    Serial.print("rightinner:");
    Serial.println(right_inner_value);
    Serial.print("rightmost:");
    Serial.println(right_most_value);
    */
    

    
    // Below is the block detection block, triggered after the third junction is encountered. 终止flag尚未设置。

    // Can we have block detect state entered at either the third junction, or a time interval after the second.?? !!!!!!!!!!

    if (block_detect_state == HIGH){
      
    }

    if(block_detect_state == HIGH){
      // Rotation state.
      if(end_of_rotation == LOW){
        if (rotate_left_flag == HIGH){
          rotate_left();
        }
        else{
          rotate_right();
        }
        // If any of the inner line sensor on the line, stop rotation and start line following. (could include the outter sensors to raise the sensitivity.)
        if(left_inner_value < left_threshold ){
          end_of_rotation = HIGH;
        }
        else if (right_inner_value < right_threshold){
           end_of_rotation = HIGH;
        }
      }
      // Line following state in the block detection.
      else{
        // Test for L junction
        if (left_most_value < left_threshold && left_inner_value < left_threshold ){
          end_of_rotation = LOW; // The left sensors on the L cornoer. Turn right
          rotate_right();
          rotate_left_flag = LOW;
          delay(1000);
        }
        // On the middle junction
        else if(right_inner_value < right_threshold && right_most_value < right_threshold){
          end_of_rotation = LOW; // Start rotation by 180 degree ad stop the line following.
          rotate_left();
          rotate_left_flag = HIGH;
          delay(1000);
        }
        else if(left_most_value < left_threshold && left_inner_value < left_threshold && right_inner_value < right_threshold && right_most_value < right_threshold){
          forwards();
        }
        else{
          line_follow();
        }
      }
      
    }
    // End of the block detection block


    
    //Below is the line following block and will be executed when the line following state is HIGH.
    //Line following function(to be executed when the line follow flag is on)
    if(line_follow_state == HIGH){
      
      
     // On the junction
     if (left_most_value < left_threshold && left_inner_value < left_threshold && right_inner_value < right_threshold && right_most_value < right_threshold) {
        //halt();
        junction_counter++;
        forwards();
        delay(1000);
      }
      else{
        line_follow();
      }
      
      if (junction_counter >= 3) {
        line_follow_state = LOW;
        block_detect_state = HIGH;
        rotate_left_flag = HIGH;
        junction_counter = 0;
        forwards();  // Move the robot forward for rotation.
        //delay(100);
        // Sign for testing
      }
      
    }
    // End of the line following block.
    
  }
  else{
    reset_wibich();
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


// Reset function
void reset_wibich(){
  halt();
  button_state = LOW;        
  reverse_state = LOW;
  line_follow_state = HIGH;
  block_detect_state = LOW;
  button_counter = 0;
  junction_counter = 0;
  end_of_rotation = LOW;
  rotate_left_flag = LOW;
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
