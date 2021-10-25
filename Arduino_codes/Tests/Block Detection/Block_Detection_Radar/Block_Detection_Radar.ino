
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
#define echoPin 9 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 10 //attach pin D3 Arduino to pin Trig of HC-SR04

// defines variables
long duration, distance_average; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int distance_array[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int block_distance_array[180];
int counter = 0;
int angle = 0;
int lowest_distance = 100;
int number_lowest_distance = 0;

// Sets the left wheel motor to pin M3 and the Right to M4
Adafruit_DCMotor *Left_Wheel_Motor = AFMS.getMotor(3);
Adafruit_DCMotor *Right_Wheel_Motor = AFMS.getMotor(4);

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed

    if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");
  Left_Wheel_Motor->setSpeed(200);
  Right_Wheel_Motor->setSpeed(200);

}
void loop() {


  //For each angle, we take the average of ten readings. The angles are just counters, it's not linked to any known data.
  if (counter == 9) {
    for (int i = 0; i <=9; i++){
      distance_average += distance_array[i];
    }
    distance_average = distance_average/10;

    // Once we have done ten readings, we move one increment
    Left_Wheel_Motor->run(FORWARD);
    Right_Wheel_Motor->run(BACKWARD);
    delay (5);
    Left_Wheel_Motor->run(RELEASE);
    Right_Wheel_Motor ->run(RELEASE);

    // Add the average in the array
    block_distance_array[angle] = distance_average;
    angle += 1;
    counter = 0; 
  }
// When we've done 180 degreees, we then see what the lowest reading was, and head towards it.
  if ((angle == 180) && (counter == 0)) {
    for (int i = 0; i <=179;i++){
      if (block_distance_array[i] < lowest_distance) {
        lowest_distance = block_distance_array[i];
      } else if (block_distance_array[i] == lowest_distance) { //If there's multiple lowest distance numbers, we find the mean angle of how many there are
        number_lowest_distance += i;
        counter += 1;
    }
    angle = number_lowest_distance/counter;
    Left_Wheel_Motor->run(BACKWARD);
    Right_Wheel_Motor->run(FORWARD);
    delay (5 * (180 - angle));
    Left_Wheel_Motor->run(RELEASE);
    Right_Wheel_Motor ->run(RELEASE);
  }
  
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH); 

  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  distance_array[counter] = distance;
  counter +=1;

  }
}
