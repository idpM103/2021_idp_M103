/*
  line follower code using line sensors
  reads the output analogue values from the two sensors.
  and compare the results with the threshold to decide later-on motion
  left sensor A0
  right sensor A1

  Created 10.10.21
  BY S.Han
  
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>

#define left_sensor_pin A0
#define right_sensor_pin A1
int left_sensor_value; //  higher lighter
int right_sensor_value;
int threshold = ; 
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
