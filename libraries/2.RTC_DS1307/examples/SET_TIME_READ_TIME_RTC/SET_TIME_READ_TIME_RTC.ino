#include "rtc_ds1307.h"

void setup() {
  Serial.begin(115200);
  Serial.println("RTC Test");

  // Initialize RTC on P5.4/P5.5
  if (RTC_begin(I2C_PINS_P54_P55)) {
    Serial.println("RTC initialized");

    // Set time: 12:30:00, Saturday, Dec 13, 2025
    RTC_setTime(25, 12, 13, 12, 30, 0, RTC_SATURDAY);
  } else {
    Serial.println("RTC not found!");
  }
}

void loop() {
  uint8_t year, month, date, hours, minutes, seconds, day;

  if (RTC_getTime(&year, &month, &date, &hours, &minutes, &seconds, &day)) {
    // Print time
    Serial.print("Time: ");
    Serial.printNumber(hours);
    Serial.print(":");
    if (minutes < 10) Serial.print("0");
    Serial.printNumber(minutes);
    Serial.print(":");
    if (seconds < 10) Serial.print("0");
    Serial.printNumber(seconds);

    // Print date
    Serial.print("  Date: ");
    Serial.printNumber(date);
    Serial.print("/");
    Serial.printNumber(month);
    Serial.print("/20");
    Serial.printNumber(year);
    Serial.println("");
  }

  delay_ms(1000);
}