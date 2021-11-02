//'FORWARDS' IN CODE = 'FORWARD' ON AGV
//remember to set D10 high while driving, low when stopped (contorls amber LED)

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <Servo.h>
// Big change in hall effect. The inout needs to be changed to digital
//Pins
#define left_most A3
#define left_inner A2 // The left inner sensor is abandoned
#define right_inner A1
#define right_most A0
const int buttonPin = 7;    // the number of the pushbutton pin
#define hall_effect 2
// pin 9 is reserved for the servo.
#define echoPin 4 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 5 //attach pin D3 Arduino to pin Trig of HC-SR04
#define metal_LED_Pin 12
#define nonmetal_LED_Pin 11

// Flags
int previous_button_value = LOW;
int current_button_value = LOW;
int button_state = LOW;        
int reverse_state = LOW;
int line_follow_state = HIGH;// Turn to high initially
int block_detect_state = LOW;//block_detect_state is somehow a flag for rotation now.
int end_of_rotation = HIGH;
int rotate_left_flag = LOW;
volatile int metal_state = LOW;
int nonmetal_ledState = LOW;
volatile int sweep_done = LOW; 
volatile int block_detected = LOW;
volatile int block_classified = LOW;
volatile int along_line = HIGH;
int block_lost = LOW;
int block_delivering = LOW;
int arm_raised;
int ready_to_sweep = LOW;
int first_sweep = HIGH;
volatile int backward_flag = LOW;
int restore_flag = LOW;


//Other variables
int button_counter = 0;
int junction_counter = 0;
int left_most_value;      // Left line sensor value-  higher number => lighter
int left_inner_value;
int right_inner_value;     // Right line sensor value
int right_most_value;
int delta_threshold = 100;// Cut-off for sensor reading being over the line or not
int left_inner_threshold = 300;
int left_most_threshold = 400;
int right_inner_threshold = 200;
int right_most_threshold = 400;


int metal_threshold = 80;
#define right_motor_speed 255     // Standard speed of motor
#define left_motor_speed 255
int distance; // variable for the distance measurement
int duration;
int hall_value;
volatile int detect_counter = 0;
int ultrasonic_current_height ; // 72mm from the design (up to 26oct)
int ultrasonic_original_height;
int servo_pos = 0;    // variable to store the servo position

// Function declarations
void line_follow();
void backwards();
void forwards ();
void halt ();
void left ();
void right ();
void rotate_right();
void rotate_left();
unsigned long prev_increment_time_0 = 0;
unsigned long prev_increment_time_1 = 0;
const long min_increment_interval = 1000;
void arm_drop();
void arm_raise();
void hall_interrupt();
// create servo object to control a servo
Servo arm_servo; 

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select motor ports
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *RightMotor = AFMS.getMotor(2);



void setup() {
  // Set up serial communication
  Serial.begin(9600);
  AFMS.begin();
  pinMode(buttonPin, INPUT);
  pinMode(hall_effect, INPUT); // Set the hall effect pin to digital input.
  pinMode(metal_LED_Pin, OUTPUT);
  pinMode(nonmetal_LED_Pin, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  //attachInterrupt(hall_effect, hall_interrupt, FALLING);// hall_interrupt参数不可以加（）因input为名字而非函数。
  //arm_servo.attach(9);  // attaches the servo on pin 9 to the servo object  The attach command is in the arm raise arm drop functions.
  arm_raise();
}





//
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




    // The restoring block, the last major block and the robot should return to the initial line following state after this block is executed.
    if(restore_flag == HIGH){
      if(end_of_rotation == LOW){
        rotate_right();
        if (right_inner_value < right_inner_threshold){
          end_of_rotation = HIGH;
          along_line = HIGH;
        }
      }
      else if(along_line == HIGH){
        line_follow();
        if ((right_inner_value < right_inner_threshold && right_most_value < right_most_threshold && left_inner_value < left_inner_threshold) || (right_inner_value < right_inner_threshold && left_most_value < left_most_threshold && left_inner_value < left_inner_threshold)){ // On the junction to the main path.
          along_line = LOW;
          forwards();
          delay(100);
        }
      }
      else{
        if (metal_state == HIGH){
          rotate_left();
          if(left_inner_value < left_inner_threshold){
            restore_flag = LOW;// Simply exit the block and move to the line following block again.
            line_follow_state = HIGH;
          }
        }
        else{
          rotate_right();
          if(right_inner_value < right_inner_threshold){
            restore_flag = LOW;
            line_follow_state = HIGH;
          }
        }
      }
    }







    
    //The block delivery block, the fourth major block.
    if(block_classified == HIGH){
      // The first stage is to find the junction of the target areas.
      if(along_line == HIGH){
        line_follow();
        // Move to the next stage on the first jucntion met. The leftmost threshold is also deleted due to the malfunction of the analogue pin.
        if ((right_inner_value < right_inner_threshold && right_most_value < right_most_threshold && left_inner_value < left_inner_threshold) || (right_inner_value < right_inner_threshold && left_most_value < left_most_threshold && left_inner_value < left_inner_threshold)){
          forwards();// Move forwards a bit for turning.
          delay(500);
          along_line = LOW;
          end_of_rotation = LOW;  // Ready to perform rotation.
          block_delivering = HIGH; // Ready to enter the next stage.
          if(metal_state == HIGH){
            rotate_right();
            delay(250);
          }
          else{
            rotate_left();
            delay(250);
          }
        }
      }
      // The second stage is the block delivering stage.
      else if(block_delivering == HIGH){
        left_inner_threshold = 700;
        left_most_threshold = 500;
        right_inner_threshold = 200;
        right_most_threshold = 500;
        // Rotate the robot to face the correct area, stop when one of the inner sensors is on the line.
        if(end_of_rotation == LOW){
          if(metal_state == HIGH){
            rotate_right();
            if (right_inner_value < right_inner_threshold){
              end_of_rotation = HIGH;
            }
          }
          else{
            rotate_left();          
            if(left_inner_value < left_inner_threshold ){
              end_of_rotation = HIGH;
            }
          }
        }
        // The robot is then facing the correct placement area.
        else{
          // At the dropping point(i.e. the junction) Note that the leftmost line sensor is still abandoned here.
          if ((right_inner_value < right_inner_threshold && right_most_value < right_most_threshold && left_inner_value < left_inner_threshold) || (right_inner_value < right_inner_threshold && left_most_value < left_most_threshold && left_inner_value < left_inner_threshold)){
            forwards();
            delay(50);// Push the block into the area.
            block_classified = LOW; // Exit the block.
            block_delivering = LOW;
            end_of_rotation = LOW; // Prepare to rotate backwards.
            restore_flag = HIGH; // Enter the next restoring block to complete one loop.
            halt();
            delay(5000);
          }
          // Move towards the dropping point.
          else{
            line_follow();
          }
        }
      }
    }
    





    // The third major block. The block detection and metal detection block. 
    if(sweep_done == HIGH){
      Serial.println("enter sweep");
      // Read the distance and the hall effect sensor value after enter this block
      hall_value = digitalRead(hall_effect);
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
      Serial.println("trigger sensors");
      //Serial.println(distance);
      // The first stage is to move the block forwards to check if there is a block collected.
      // Remember to turn on the along line flag after sweeping.
      if (along_line == HIGH){
        Serial.println("trigger line following");
        line_follow();
        delay(10); // Delay every 0.01s.  
        //if(detect_counter == 0){
        //  ultrasonic_original_height = distance;
        //}
        // Block detected  Assume that the block is always collected.
        if (distance<=6){   //Threshold height to trigger the block detector i.e. the ultrasonic sensor.
          Serial.println("block found");
          Serial.println(distance);
          along_line = LOW;
          block_detected = HIGH; // To go to the next step.
          detect_counter = 0;
          forwards();
          delay(500); // Adjust this delay to ensure that the metal detector is in contact with the block.
          halt();
          backward_flag = HIGH; // Ready to move the robot backward to try to trigger the metal detector.
          //Serial.println("block detected");
        }
      }
      // The second stage is to metal-detect the block, and will only be executed after the first stage is executed.
      // The assumption made is that the block will not be lost in the metal detection state.
      else if(block_detected == HIGH){
        Serial.println("moving backwards");
        // Move the robot backwards to try to pull the magent up to trigger reading.
        if(backward_flag == HIGH){
          LeftMotor->setSpeed(0.5*left_motor_speed);
          RightMotor->setSpeed(0.5*right_motor_speed);
          backwards();
          delay(50);
          halt();
          delay(50);
          detect_counter ++;
        }
        if(hall_value == LOW){
          halt();
          LeftMotor->setSpeed(left_motor_speed);
          RightMotor->setSpeed(right_motor_speed);
          metal_state = HIGH;
          digitalWrite(metal_LED_Pin, HIGH);
          delay(5000); // Delay for 5 seconds to indicate that a block with metal is detected.
          digitalWrite(metal_LED_Pin, LOW);
          sweep_done = LOW;// Leave the block
          block_detected = LOW;
          block_classified = HIGH;
          along_line = HIGH;// Follow the line in the next block.
          backward_flag = LOW;
          detect_counter = 0;
          forwards();
        }
        // The metal detected part is moved to ISR.
        if(detect_counter>=50){ // Out of the reasonable distance.
          halt();
          LeftMotor->setSpeed(left_motor_speed);
          RightMotor->setSpeed(right_motor_speed);
          metal_state = LOW;
          digitalWrite(nonmetal_LED_Pin, HIGH);   
          delay(5000);
          digitalWrite(nonmetal_LED_Pin, LOW);
          sweep_done = LOW;
          block_detected = LOW;
          block_classified = HIGH;
          along_line = HIGH;
          backward_flag = LOW;
          detect_counter = 0;
          forwards();
        }
      }
    }







// The sencond major block, block sweeping. Work reasonably well.
    if(block_detect_state == HIGH){
      if (ready_to_sweep == LOW && first_sweep == HIGH){
       backwards();
       delay(1800);
       halt();
       delay(100);
       rotate_left();// Ensure that it rotates to the correct angle.
       delay(1500);
       halt();
       delay(100);
       arm_drop();
       ready_to_sweep = HIGH;
      }
      else if(ready_to_sweep == HIGH && first_sweep == HIGH){
        rotate_right();
        delay(3000);  // Ensure that it sweeps over the whole area
        first_sweep = LOW;
        end_of_rotation = LOW;
      }
      else{
        if(end_of_rotation == LOW){
          rotate_right();
          if (right_inner_value < right_inner_threshold){// The right inner sensor on the line.
               end_of_rotation = HIGH;
               halt();
          }
        }
        else{ // Restore all the flags and exit the functional block.
          block_detect_state = LOW;// Leave the functional block.
          first_sweep = HIGH;
          ready_to_sweep = LOW;
          sweep_done = HIGH;// Trigger the next block.
          along_line = HIGH;
          arm_raise();
          forwards();
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




  /*  
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
*/



    
    //Below is the line following block and will be executed when the line following state is HIGH.
    //Line following function(to be executed when the line follow flag is on)
    if(line_follow_state == HIGH){
     // On the junction. The left most sensor is deleted due to the malfunction of the corresponding analogue pin.
     if ((right_inner_value < right_inner_threshold && right_most_value < right_most_threshold && left_inner_value < left_inner_threshold) || (right_inner_value < right_inner_threshold && left_most_value < left_most_threshold && left_inner_value < left_inner_threshold)) {
        //halt();
        junction_counter++;
        Serial.println(right_most_value);
         Serial.println(right_inner_value);
          Serial.println(left_inner_value);
           Serial.println(left_most_value);
        forwards();
        delay(1000);
      }
      else{
        line_follow();
      }
      
      if (junction_counter >= 3) {
        halt();
        line_follow_state = LOW;
        block_detect_state = HIGH;
        junction_counter = 0;
        delay(100);
        // Sign for testing
      }
      
    }
    // End of the line following block.
    
  }
  else{
    reset_wibich();
  }
}







/*
//ISR for the metal detector(hall effect input) No return nor input.
void hall_interrupt(){
  // Only triggerd in the particular block.
  if (sweep_done == HIGH && along_line == LOW && block_detected == HIGH){
    metal_state = HIGH;
    digitalWrite(metal_LED_Pin, HIGH);
    delay(5000); // Delay for 5 seconds to indicate that a block with metal is detected.
    digitalWrite(metal_LED_Pin, LOW);
    sweep_done = LOW;// Leave the block
    block_detected = LOW;
    block_classified = HIGH;
    along_line = HIGH;// Follow the line in the next block.
    backward_flag = LOW;
    detect_counter = 0;
  }
}
*/
// Below are the motor functions.
void backwards(){
  LeftMotor->setSpeed(left_motor_speed);
  RightMotor->setSpeed(right_motor_speed);
  LeftMotor->run(BACKWARD);
  RightMotor->run(BACKWARD);
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
// Flags;       
  reverse_state = LOW;
  line_follow_state = HIGH;// Turn to high initially
  block_detect_state = LOW;//block_detect_state is somehow a flag for rotation now.
  end_of_rotation = HIGH;
  rotate_left_flag = LOW;
  metal_state = LOW;
  nonmetal_ledState = LOW;
  sweep_done = LOW; 
  block_detected = LOW;
  block_classified = LOW;
  along_line = HIGH;
  block_lost = LOW;
  block_delivering = LOW;
  arm_raised;
  ready_to_sweep = LOW;
  first_sweep = HIGH;
  backward_flag = LOW;
  restore_flag = LOW;
  
  //Other variables
  button_counter = 0;
  junction_counter = 0;
  delta_threshold = 100;// Cut-off for sensor reading being over the line or not
  metal_threshold = 80;
  detect_counter = 0;
  servo_pos = 0;    // variable to store the servo position
left_inner_threshold = 600;
left_most_threshold = 100;
right_inner_threshold = 100;
right_most_threshold = 150;
}


// Line follow function. Simply follow a straight line.
void line_follow(void){
    if (left_most_value > left_most_threshold && left_inner_value > left_inner_threshold && right_inner_value > right_inner_threshold && right_most_value > right_most_threshold) {
      forwards();
    }
    //Right inner sensor on the line (Small right turn)
    else if (left_most_value > left_most_threshold && left_inner_value > left_inner_threshold && right_inner_value < right_inner_threshold && right_most_value > right_most_threshold ) {
      if (reverse_state == LOW){
        right();
        delay(50);
      }
    }
    //Left inner sensor on the line  (Small left turn)
    else if (left_most_value > left_most_threshold && left_inner_value < left_inner_threshold && right_inner_value > right_inner_threshold && right_most_value > right_most_threshold ) {
      if (reverse_state == LOW){
        left();
      }
      
    }
    //Right outer sensor on the line (Large right turn)
    else if (left_most_value > left_most_threshold && left_inner_value > left_inner_threshold && right_inner_value > right_inner_threshold && right_most_value < right_most_threshold) {
      right();
      delay(250);
    }
    //Left outer sensor on the line (Large left turn)
    else if (left_most_value < left_most_threshold && left_inner_value > left_inner_threshold && right_inner_value > right_inner_threshold && right_most_value > right_most_threshold) {
      left();
      
    }
}


// Servo Control
// Note that the boudaries in the functions below need to match each other.
void arm_drop(){
  // Raises the arm - we'll need to find the right position value
  arm_servo.attach(9);
  for (servo_pos = 60; servo_pos <= 180; servo_pos += 1) { // goes from 
    arm_servo.write(servo_pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       
  }
  arm_servo.detach();
  arm_raised = LOW;
}

void arm_raise(){
  // Drops the arm - we'll need to find the right position value
  arm_servo.attach(9);
  for (servo_pos = 180; servo_pos >= 60; servo_pos -= 1) { // 
    arm_servo.write(servo_pos);              // tell servo to go to position in variable 'pos'
    delay(15);          
  }             
  arm_servo.detach();
  arm_raised = HIGH;
}

void final_delivery_right(){
  if(counter < 2)
  {
    rotate_right();
    if(left_most_value < left_most_threshold)
    {
      counter ++;
      delay(250);
    }
    
  }
  else{
    rotate_left();
    delay(200);
    forwards();
    delay(1250);
    backwards();
    delay(1500);
    rotate_right();
    delay(1500);
    halt();
    delay(10000);
  }
}
void final_delivery_left(){
  if(counter < 2)
  {
    rotate_left();
    if(right_most_value < right_most_threshold)
    {
      counter ++;
      delay(250);
    }
    
  }
  else{
    rotate_right();
    delay(200);
    forwards();
    delay(1250);
    backwards();
    delay(1500);
    rotate_left();
    delay(1500);
  }
}
