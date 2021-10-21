// Ultrasonic Sensor testing code. Written by a 13 year old.
#include <NewPing.h> // Imports the NewPing Library.
int trigPin = 10; // Add the Trig pin on pin 10.
int echoPin = 9; // Add the ECHO pin on pin 9.
int duration, distance; // Add types 'duration' and 'distance'.

void setup()
{
pinMode (trigPin, OUTPUT);// Same as above, the TRIG pin will send the ultrasonic wave.
pinMode (echoPin, INPUT); // The ECHO pin will recieve the rebounded wave, so it must be an input type.

}

void loop()
{
digitalWrite (trigPin, HIGH);
delay(50);
digitalWrite (trigPin, LOW);
duration=pulseIn(echoPin,HIGH);
distance=(duration/2)/29.1;


Serial.println(distance);
Serial.print("cm");
delay(50);
}
