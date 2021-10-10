/* This code is to be used with KY-024 Hall effect sensor
 * It displays both Analog and Digital values given by the sensor
 * Refer to www.surtrtech.com for more details
 */


void setup() {
  Serial.begin(9600);
}

void loop() {
  
   //read on A0
   int sensorValue = analogRead(A0);
   // print out value
   Serial.println(sensorValue);
   
   delay(1);
}
