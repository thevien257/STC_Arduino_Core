/*
  Blink
  Turns an LED on for one second, then off for one second, repeatedly.
  
  This example code is in the public domain.
*/

void setup() {
  pinMode(P5_5, OUTPUT);
}

void loop() {
  digitalWrite(P5_5, HIGH);
  delay_ms(1000);
  digitalWrite(P5_5, LOW);
  delay_ms(1000);
}