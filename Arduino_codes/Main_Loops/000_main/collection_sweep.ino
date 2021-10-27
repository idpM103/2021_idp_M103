

void arm_drop(){
  // Drops the arm - we'll need to find the right position value
  arm_servo.attach(10);
  for (servo_pos = 180; servo_pos >= 90; servo_pos -= 1) { // goes from 180 degrees to 0 degrees
    arm_servo.write(servo_pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15 ms for the servo to reach the position
  }
  arm_servo.detach();
  delay(100);
  arm_raised = LOW;
}

void arm_raise(){
  // Raises the arm - we'll need to find the right position value
  arm_servo.attach(10);
  for (servo_pos = 90; servo_pos <= 180; servo_pos += 1) { // goes from 180 degrees to 0 degrees
    arm_servo.write(servo_pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15 ms for the servo to reach the position
  }
  arm_servo.detach();
  delay(100);
  arm_raised = HIGH;
}

// When collection starts, we want to be on the third junction
void collection_sweep(){
  
  reverse_state = HIGH;
  forwards();
  delay(750); // We reverse a bit
  halt();
  reverse_state = LOW;

  rotate_left();
  delay(750); // We turn left a bit
  halt();

  // We drop the arm
  if (arm_raised == HIGH) {
    arm_drop();
  }
  
  rotate_right();
  delay(2250); // We turn left a lot (through the entire square), but not 180 yet
  halt();

  arm_raise();
  // This should tell when we're on the line? !!!!!!!
  /*
  right_most_value  = analogRead(right_most);
  while (right_most_value > right_threshold) {
    rotate_right();
    delay(50);
    halt();
    right_most_value  = analogRead(right_most);
  }
  */
  along_line = HIGH;
  
  // There's a chance we need to do a 360 turn, then move forward a bit, then turn again - this should be simple as long as we can tell when we're on the line
  // Then we detect the block !!
}
