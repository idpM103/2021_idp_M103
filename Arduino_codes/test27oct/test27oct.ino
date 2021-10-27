//'FORWARDS' IN CODE = 'FORWARD' ON AGV
//remember to set D10 high while driving, low when stopped (contorls amber LED)

#include <Wire.h>
#include <Adafruit_MotorShield.h>

//Pins
#define left_most A4
#define left_inner A3
#define right_inner A2
#define right_most A1
const int buttonPin = 7;    // the number of the pushbutton pin
#define hall_effect A0
#define echoPin 8 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 10 //attach pin D3 Arduino to pin Trig of HC-SR04
#define metal_LED_Pin 12
#define nonmetal_LED_Pin 11

// Flags
int previous_button_value = LOW;
int current_button_value = LOW;
int button_state = LOW;        
int reverse_state = LOW;
int line_follow_state = LOW;// Turn to high initially
int block_detect_state = LOW;//block_detect_state is somehow a flag for rotation now.
int end_of_rotation = HIGH;
int rotate_left_flag = LOW;
int metal_state = LOW;
int nonmetal_ledState = LOW;
int sweep_done = HIGH; 
int block_detected = LOW;
int block_classified = LOW;
int along_line = HIGH;
int block_lost = LOW;
int block_delivering = LOW;

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
int metal_threshold = 20;
#define right_motor_speed 255     // Standard speed of motor
#define left_motor_speed 255
int distance; // variable for the distance measurement
int duration;
int hall_value;
int detect_counter = 0;
int ultrasonic_current_height ; // 72mm from the design (up to 26oct)
int ultrasonic_original_height;

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

void setup() {
  // Set up serial communication
  Serial.begin(9600);
  AFMS.begin();
  pinMode(buttonPin, INPUT);
  pinMode(metal_LED_Pin, OUTPUT);
  pinMode(nonmetal_LED_Pin, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
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
    

    // The block detection and metal detection block.
    if(sweep_done == HIGH){
      // Read the distance and the hall effect sensor value after enter this block
      hall_value = analogRead(hall_effect);
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
       // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(echoPin, HIGH); 
      // Calculating the distance
      distance = duration * 0.034 / 2;
      //Serial.println(distance);
      // The first stage is to move the block forwards to check if there is a block collected.
      // Remember to turn on the along line flag after sweeping.
      if (along_line == HIGH){
        line_follow();
        delay(10); // Delay every 0.01s.  
        //if(detect_counter == 0){
        //  ultrasonic_original_height = distance;
        //}
        detect_counter ++;
        // Block detected
        if (distance<=5){   //Threshold height
          sweep_done = LOW; // Turn off the flag to exit the block.
          along_line = LOW;
          block_detected = HIGH;
          detect_counter = 0;
          Serial.println("block detected");
        }
        
        // Adjust the counter limit to longer the attempt time (0.01 N seconds) The block is regarded as lost if the limit is exceeded.
        if (detect_counter >= 500000){
          //Block lost. Get out of this function block.
          detect_counter = 0;
          sweep_done = LOW;
          along_line = LOW;
          block_lost = HIGH;
        }
        
      }

      
      // The second stage is to metal-detect the block, and will only be executed after the first stage is executed.
      // The assumption made is that the block will not be lost in the metal detection state.
      else if(block_detected == HIGH){
        // Keep driving the block forwards to make the block in contact with the metal detector.
        forwards();
        delay(500); // Adjust this delay to ensure that the metal detector is in contact with the block.
        halt();
        delay(500);
        if(hall_value >= metal_threshold){
          metal_state = HIGH;
          digitalWrite(metal_LED_Pin, HIGH);
          delay(5000); // Delay for 5 seconds to indicate that a block with metal is detected.
          digitalWrite(metal_LED_Pin, LOW);
          sweep_done = LOW;// Leave the block
          block_detected = LOW;
          block_classified = HIGH;
          along_line = HIGH;// Follow the line in the next block.
          Serial.println("metal detected");
        }
        else{
          metal_state = LOW;
          digitalWrite(nonmetal_LED_Pin, HIGH);   
          delay(5000);
          digitalWrite(nonmetal_LED_Pin, LOW);
          sweep_done = LOW;
          block_detected = LOW;
          block_classified = HIGH;
          along_line = HIGH;
        }
      }
    }

/*
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

*/



    //The block delivery block.
    if(block_classified == HIGH){
      // The first stage is to find the junction of the target areas.
      if(along_line == HIGH){
        line_follow();
        // Move to the next stage on the first jucntion met.
        if (left_most_value < left_threshold && left_inner_value < left_threshold && right_inner_value < right_threshold && right_most_value < right_threshold){
          forwards();// Move forwards a bit for turning.
          delay(50);
          along_line = LOW;
          end_of_rotation = LOW;
          block_delivering = HIGH; // Ready to enter the next stage.
          if(metal_state == HIGH){
            rotate_right();
            delay(50);
          }
          else{
            rotate_left();
            delay(50);
          }
        }
      }
      // The second stage is the block delivering stage.
      else if(block_delivering == HIGH){
        // Rotate the robot to face the correct area.
        if(end_of_rotation == LOW){
          if(metal_state == HIGH){
            rotate_right();
          }
          else{
            rotate_left();
          }
          if(left_inner_value < left_threshold ){
            end_of_rotation = HIGH;
          }
          else if (right_inner_value < right_threshold){
            end_of_rotation = HIGH;
          }
        }
        else{
          // At the dropping point
          if (left_most_value < left_threshold && left_inner_value < left_threshold && right_inner_value < right_threshold && right_most_value < right_threshold){
            block_classified = LOW; // Leave the block.
            block_delivering = LOW;
            halt();
          }
          // Move towards the dropping point.
          else{
            line_follow();
          }
        }
      }
    }
    




    
    //Below is now a rotation block. block_detect_state is somehow a flag for rotation now.
    if(block_detect_state == HIGH){
      // Rotation state.
      // Rotation continues until one of the inner line sensors is on the line.
      if(end_of_rotation == LOW){
        if (rotate_left_flag == HIGH){
          rotate_left();
        }
        else{
          rotate_right();
        }
        // If any of the inner line sensor on the line, stop rotation and start line following. (could include the outter sensors to raise the sensitivity.)
        if(left_inner_value < left_threshold ){
          end_of_rotation = HIGH;  // Cannot be encapsulated into a function since in a fcuntion the change in a variable is temporary (only a change to the copy).
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
  //Serial.println("turning right ");
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
  //Serial.println("turning left ");
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
  //Serial.println("stopping ");
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
