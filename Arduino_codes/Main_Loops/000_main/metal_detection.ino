#include <Wire.h>
#define hall_effect A4
#define echoPin 9 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 10 //attach pin D3 Arduino to pin Trig of HC-SR04
#define metal_LED_Pin 5
#define nonmetal_LED_Pin 6


const int ultrasonic_height = 7.2 ; // 72mm from the design (up to 26oct)
const int metal_threshold = 1.5; // unknown threshold for the hall effectors
int metal_state = LOW;
int metal_ledState = LOW;
int nonmetal_ledState = LOW;
int sweep_done = HIGH; // assume first sweep finished
int block_detected = LOW;
int distance; // variable for the distance measurement
int hall_value;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(metal_LED_Pin, OUTPUT);
  pinMode(nonmetal_LED_Pin, OUTPUT);
  

}

void loop() {
  // put your main code here, to run repeatedly:
  if (sweep_done == HIGH){
  //finish the sweep for fixing the orientation
  junction_counter = 0;
  forwards();
  delay(30);
  //drive wibich to the block for further detection
  halt();
  digitalWrite (trigPin, HIGH);
  delay(50);
  digitalWrite (trigPin, LOW);
  duration=pulseIn(echoPin,HIGH);
  distance=(duration/2)/29.1;
  
  if (distance < ultrasonic_height){
  // need dimension from mechanical team  
    
    block_detected = HIGH;
    
  } else {
    
    block_detected = LOW;

    }

  if (block_detected == HIGH) {
    if (halt_value >= metal_threshold) {
      
      metal_state = HIGH;
      //metal_ledState = metal_state;
      digitalWrite(metal_LED_Pin, HIGH);
            
    } else{    
      
      metal_state = LOW;
      //nonmetal_ledState =! metal_state;
      digitalWrite(nonmetal_LED_Pin, HIGH);   
         
    }
  } else {    
    
    rotate_left();
    line_follow();
    // no block detected follow the line back to the junction, for another sweep
    
    }   
  }

}
