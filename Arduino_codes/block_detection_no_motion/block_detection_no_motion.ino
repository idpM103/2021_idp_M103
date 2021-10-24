#include <NewPing.h> 

const int front_ultrasonic_trigPin = 8;
const int front_ultrasonic_echoPin = 9;
const int side_ultrasonic_trigPin = 10;
const int side_ultrasonic_echoPin = 11;
int duration, distance; // Add types 'duration' and 'distance'.

void detect();
int block_detect_state = HIGH;
int block_find_state = LOW;
void setup() {
  // put your setup code here, to run once:
  pinMode (side_ultrasonic_trigPin, OUTPUT);// Same as above, the TRIG pin will send the ultrasonic wave.
  pinMode (side_ultrasonic_echoPin, INPUT); // The ECHO pin will recieve the rebounded wave, so it must be an input type.

}

void loop() {
  // put your main code here, to run repeatedly:
  if (block_detect_state == HIGH){
    detect();
  } else{
    // line_follow();
  }

}

void detect(void){
  if (block_find_state == LOW){
    digitalWrite (side_ultrasonic_trigPin, HIGH);
    delay(50);
    digitalWrite (side_ultrasonic_trigPin, LOW);
    duration=pulseIn(side_ultrasonic_echoPin,HIGH);
    distance=(duration/2)/29.1;
    if (distance < 35){
      block_find_state = HIGH;
    } else {
      block_find_state = LOW;
    }
//dk exact distance need measurement and testings. 35 is the rough value of the square
//might need to add motor
  }


  
}
