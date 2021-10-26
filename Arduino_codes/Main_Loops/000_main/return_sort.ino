void return_sort();
void back_home();
int block_counter = 0;
int home_state = LOW;

void return_sort(){
  if (block_detected == HIGH && block_classified == HIGH){
    // block is detected and classified
    
    line_follow_state == HIGH;
    
    if (block_counter < 6){
      
      home_state = LOW;
      //haven't received all 6 blocks yet
      
      if (junction_counter == 1){
        if (metal_state == HIGH){
          // place the metal block into the red area
          right();
          line_follow();
          if (junction_counter == 2){
            forwards();
            delay(200);
            junction_counter = 0;
            
          }
        } else {
          
          // place the non-metal block into the blue area        
          left();
          line_follow();
          if (junction_counter == 2){
            forwards();
            delay(200);
            junction_counter = 0;
            
          }          
        }
      }
      block_counter++;
      //count the block collected and sorted
    } else {
      home_state = HIGH;
      // all 6 blocks collected
    }
  }
}

void back_home(){
  if (metal_state == HIGH){
    // can we do backward?
    // rotate might sweep the positioned block
    reverse_state = HIGH;
    forwards();
    if (left_most_value < left_threshold && left_inner_value < left_threshold && right_inner_value < right_threshold && right_most_value < right_threshold){
      junction_counter++;
    }
    line_follow_state = HIGH;
    if (junction_counter == 2){
      rotate_left();
    }
    if (junction_counter == 3){
      forwards();
      delay(100);
      halt();
      //arrive at the start square from metal region
    }
  } else {
    reverse_state = HIGH;
    forwards();
    if (left_most_value < left_threshold && left_inner_value < left_threshold && right_inner_value < right_threshold && right_most_value < right_threshold){
      junction_counter++;
    }
    line_follow_state = HIGH;
    if (junction_counter == 2){
      rotate_right();
    }
    if (junction_counter == 3){
      forwards();
      delay(100);
      halt();
      //arrive at the start square from non-metal region   
    }
  }
}
