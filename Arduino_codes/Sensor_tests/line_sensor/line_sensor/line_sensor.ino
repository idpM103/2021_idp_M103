/*
  Line sensor testing code
  reads the output analogue values from the two sensors.
  Record threshold values for later use

  left sensor A0
  right sensor A1

  Created 10.10.21
  BY S.Han
  
*/

#include <Wire.h>

#define left_sensor_pin A0
#define right_sensor_pin A1
int left_sensor_value; //  higher lighter
int right_sensor_value;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  left_sensor_value = analogRead(left_sensor_pin);
  right_sensor_value =  analogRead(right_sensor_pin);
  Serial.print("Left: ");
  Serial.print(left_sensor_value);
  Serial.print("Right: ");
  Serial.println(right_sensor_value); 
}
