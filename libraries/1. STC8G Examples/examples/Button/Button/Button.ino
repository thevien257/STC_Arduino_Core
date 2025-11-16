/*
  Button
  Reads a digital input on pin P3_2, controls LED on P5_5
  
  This example code is in the public domain.
*/

void setup() {
  Serial.begin(115200);
  pinMode(P3_2, INPUT_PULLUP);
  pinMode(P5_5, OUTPUT);
}

void loop() {
  if (!digitalRead(P3_2)) {
    digitalWrite(P5_5, HIGH);
    Serial.println("Button Pressed");
  } else {
    digitalWrite(P5_5, LOW);
  }
}