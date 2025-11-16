/*
  Blink Without Delay
  Uses micros() or millis() for non-blocking LED blinking
  
  This example code is in the public domain.
*/

// Pin definitions
#define LED_PIN P5_5

void setup() {
  Serial.begin(9600);

  // Set LED pin as output
  pinMode(LED_PIN, OUTPUT);

  // Start with LED off
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  static uint32_t previousMicros = 0;
  uint32_t currentMicros = micros();

  // Alternative: Use millis() for millisecond-level timing
  // static uint32_t previousMillis = 0;
  // uint32_t currentMillis = millis();

  // Blink every 1 second (1,000,000 microseconds)
  if ((currentMicros - previousMicros) >= 1000000UL) {
    Serial.println("Toggle LED");
    previousMicros = currentMicros;
    digitalToggle(LED_PIN);
  }

  // Alternative using millis():
  // if ((currentMillis - previousMillis) >= 1000) {
  //   Serial.println("Toggle LED");
  //   previousMillis = currentMillis;
  //   digitalToggle(LED_PIN);
  // }
}