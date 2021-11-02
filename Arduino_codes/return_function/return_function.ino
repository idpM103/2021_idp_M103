int return_counter = 0;
//int block_delivering = LOW;
int line_follow_state = LOW; 
int junction_counter = 0;
volatile int sweep_done = LOW;
volatile int return_state = LOW;
void return ();


void return() {
  // put your setup code here, to run once:
  if (sweep_done == HIGH){
    
    if (junction_counter < 3) {
      
      if ((right_inner_value < right_inner_threshold && right_most_value < right_most_threshold && left_inner_value < left_inner_threshold) || (right_inner_value < right_inner_threshold && left_most_value < left_most_threshold && left_inner_value < left_inner_threshold)){
      
      line_follow();
      
      junction_counter++      
      
      } else {        
      
        forwards();
        
        delay(2000);        
      } 
    }
  } 
}
