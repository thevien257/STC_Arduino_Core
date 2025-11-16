/*
  Serial Echo
  Echoes received serial data back and demonstrates serial functions
  
  This example code is in the public domain.
*/

void setup() {
  Serial.begin(115200);  // TX=P3.1, RX=P3.0

  // Alternative pins (format: TX, RX):
  // Serial.beginWithPins(115200, UART_PINS_DEFAULT);  // TX=P3.1, RX=P3.0
  // Serial.beginWithPins(115200, UART_PINS_P32_P33);  // TX=P3.3, RX=P3.2
  // Serial.beginWithPins(115200, UART_PINS_P54_P55);  // TX=P5.5, RX=P5.4

  Serial.println("STC8G1K08A Serial Test");

  pinMode(P3_2, INPUT_PULLUP);
  pinMode(P5_5, OUTPUT);
}

void loop() {
  // Echo received characters
  if (Serial.available() > 0) {
    Serial.println(Serial.readLine());
  }

  // Print example data
  Serial.print("Temperature: ");
  Serial.printNumber(25);
  Serial.println("C");

  // Button control
  if (!digitalRead(P3_2)) {
    Serial.println("Button pressed!");
    digitalWrite(P5_5, HIGH);
  } else {
    digitalWrite(P5_5, LOW);
  }

  delay_ms(500);
}