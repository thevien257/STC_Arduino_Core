/*
  Button Interrupt
  Uses external interrupt to detect button press on P3_2
  
  This example code is in the public domain.
*/

volatile uint8_t button_pressed = 0;

void button_isr() {
  button_pressed = 1;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Interrupt Test");

  // Note: P3_2 and P3_3 can using FALLING, RISING or CHANGE
  // Other pins only support FALLING
  // Configure pin as input with pullup
  pinMode(P3_2, INPUT_PULLUP);

  // Attach interrupt on P3.2 (INT0)
  // FALLING: Triggers only on falling edge (button press)
  // CHANGE: Triggers on both rising and falling edges
  attachInterrupt(digitalPinToInterrupt(P3_2), button_isr, FALLING);

  // Example using other pins:
  // pinMode(P3_3, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(P3_3), button_isr, CHANGE);
}

void loop() {
  if (button_pressed) {
    button_pressed = 0;
    Serial.println("Button pressed!");
  }
  delay_ms(100);
}