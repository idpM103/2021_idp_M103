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

// defines variables
long duration, distance_average; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int distance_array[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int counter;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  counter = 0;
}
void loop() {
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
  if (counter < 9) {
    distance_array[counter] = distance;
    counter +=1;
    // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  } else {
  
      for (int i = 0; i <= 8; i++){
        distance_array[i] = distance_array[i+1];
      }
      distance_array[9] = distance;
      distance_average = 0;
      for (int i = 0; i <=9; i++){
        distance_average += distance_array[i];
      }
      distance_average = distance_average/10;
      
      // Displays the distance on the Serial Monitor
      Serial.print("Distance: ");
      Serial.print(distance_average);
      Serial.println(" cm");
  }
}
