/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground through 220 ohm resistor
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/
// constants won't change. They're used here to set pin numbers:
const int buttonPin = 7;     // the number of the pushbutton pin

// variables will change:
int buttonState = HIGH;         // variable for reading the pushbutton status
int toggle_state = LOW;
void setup() {
  Serial.begin(9600);   
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // If the button is pressed, then it checks which state the toggle is currently in, switches it, and delays
  if (buttonState == LOW) {
    if (toggle_state == LOW) {
      toggle_state = HIGH;
      Serial.println("Toggle ON");
    } else {
      toggle_state = LOW;
      Serial.println("Toggle OFF");
    }
   delay(1000); 
  }
  delay(100);
  }
