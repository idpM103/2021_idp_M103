

void arm_drop(){
  // Drops the arm - we'll need to find the right position value
  servo_pos = 180;
  arm_servo.write(servo_pos);
  delay(1000);
  arm_raised = LOW;
}

void arm_raise(){
  // Raises the arm - we'll need to find the right position value
  servo_pos = 0;
  arm_servo.write(servo_pos);
  delay(1000);
  arm_raised = HIGH;
}

// When collection starts, we want to be on the third junction
void collection_sweep(){
  
  reverse_state = HIGH;
  forwards();
  delay(500); // We reverse a bit
  halt();
  reverse_state = LOW;
  
  rotate_right();
  delay(500); // We turn right a bit
  halt();

  // We drop the arm
  if (arm_raised == HIGH) {
    arm_drop();
  }

  rotate_left();
  delay(1000); // We turn left a lot (through the entire square)
  halt();
  
}

void collection_sweep_repeat() {
  
}
