#include <Wire.h>
#define hall_effect A4

int metal_state = LOW;
int hall_value;
//int metal_threshold = ;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  hall_value = analogRead(hall_effect);
  Serial.print(hall_value);
  /*if (halt_value >= metal_threshold){
   * Serial.print(metal detected);
   * metal_state = HIGH;
   * } else{
   * Serial.print(non-metal detected);
   * metal_state = LOW;
   * }
   */
}
