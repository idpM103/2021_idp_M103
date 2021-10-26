void setup() {
  // put your setup code here, to run once:
  TCCR1A = 0;
  TCCR1B = 0;
  bitSet(TCCR1B, CS12);  // 256 prescaler
  OCR1A = 62500/4; // This should toggle the LED every quarter second, meaning it's on for half every second (2HZ)
  bitSet(TCCR1A, COM1A0); // Toggle pin OC1A (9)
  pinMode(9, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}
