void setup() {
  // put your setup code here, to run once:
  TCB0.CTRLB = TCB_CNTMODE_INT_gc; // Use timer compare mode
  TCB0.CCMP = 25000*2.5; // Value to compare with. This is 1/10th of the tick rate, so 10 Hz
  TCB0.INTCTRL = TCB_CAPT_bm; // Enable the interrupt
  TCB0.CTRLA = TCB_CLKSEL_CLKTCA_gc | TCB_ENABLE_bm; // Use Timer A as clock, enable timer
  
  // And the interrupt handler:
  
  ISR(TCB0_INT_vect)
  {
  // Do something
  
  // Clear interrupt flag
  TCB0.INTFLAGS = TCB_CAPT_bm;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
