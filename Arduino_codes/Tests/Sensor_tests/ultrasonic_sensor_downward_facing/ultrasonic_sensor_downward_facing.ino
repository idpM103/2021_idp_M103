// Ultrasonic Sensor testing code. Written by a 13 year old.
// ---------------------------------------------------------------- //
// Arduino Ultrasoninc Sensor HC-SR04
// Re-writed by Arbi Abdul Jabbaar
// Using Arduino IDE 1.8.7
// Using HC-SR04 Module
// Tested on 17 September 2019
// ---------------------------------------------------------------- //

#define echoPin 9 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 10 //attach pin D3 Arduino to pin Trig of HC-SR04

// Block flag
int block_detected = LOW;

int get_distance();

// defines variables
long duration, distance_average; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int block_threshold = 7; // How high the block height should be

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
}
void loop() {
  // Clears the trigPin condition
      for (int i = 0; i <= 9; i++){
        distance += get_distance();
      }
      distance_average = distance/10;

      if (distance_average <= block_threshold){
        block_detected = HIGH;
      } else {
        block_detected = LOW;
      }
        
      // Displays the distance on the Serial Monitor
      Serial.print("Distance: ");
      Serial.print(distance_average);
      Serial.println("cm");
      Serial.print("Block Detected: ");
      Serial.println(block_detected);
      Serial.println("");
  }

int get_distance() {
    digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH); 

  // Calculating the distance
  return(duration * 0.034 / 2); // Speed of sound wave divided by 2 (go and back)
  
}
