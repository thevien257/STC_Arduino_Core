#include "rtc_ds1307.h"

// Internal state
static __xdata bool rtc_initialized = false;

// BCD conversion helpers
uint8_t bcd_to_dec(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t dec_to_bcd(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

// Initialize RTC with I2C pins
bool RTC_begin(I2cPinSelect_t pins) {
    // Initialize I2C
    Wire.beginWithPins(pins);
    
    // Test communication with RTC
    Wire.beginTransmission(RTC_ADDRESS);
    uint8_t error = Wire.endTransmission(true);
    
    if (error == 0) {
        rtc_initialized = true;
        return true;
    }
    
    return false;
}

// Set RTC time
bool RTC_setTime(uint8_t year, uint8_t month, uint8_t date, 
                 uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day) {
    if (!rtc_initialized) return false;
    
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x00);  // Start at seconds register
    Wire.write(dec_to_bcd(seconds) & 0x7F);  // Clear CH bit to start clock
    Wire.write(dec_to_bcd(minutes));
    Wire.write(dec_to_bcd(hours) & 0x3F);    // 24-hour mode
    Wire.write(dec_to_bcd(day));
    Wire.write(dec_to_bcd(date));
    Wire.write(dec_to_bcd(month));
    Wire.write(dec_to_bcd(year));
    
    return (Wire.endTransmission(true) == 0);
}

// Get RTC time
bool RTC_getTime(uint8_t *year, uint8_t *month, uint8_t *date, 
                 uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *day) {
    uint8_t data[7];
    uint8_t i;
    
    if (!rtc_initialized) return false;
    
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x00);  // Start at seconds register
    Wire.endTransmission(false);
    
    if (Wire.requestFrom(RTC_ADDRESS, 7, true) < 7) {
        return false;
    }
    
    // Read all data into temporary array first
    for (i = 0; i < 7; i++) {
        data[i] = Wire.read();
    }
    
    // Convert from BCD and store in output variables
    *seconds = bcd_to_dec(data[0] & 0x7F);
    *minutes = bcd_to_dec(data[1] & 0x7F);
    *hours   = bcd_to_dec(data[2] & 0x3F);
    *day     = bcd_to_dec(data[3] & 0x07);
    *date    = bcd_to_dec(data[4] & 0x3F);
    *month   = bcd_to_dec(data[5] & 0x1F);
    *year    = bcd_to_dec(data[6]);
    
    return true;
}

// Check if RTC oscillator is running
bool RTC_isRunning(void) {
    if (!rtc_initialized) return false;
    
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x00);  // Seconds register
    Wire.endTransmission(false);
    
    if (Wire.requestFrom(RTC_ADDRESS, 1, true) < 1) {
        return false;
    }
    
    uint8_t seconds = Wire.read();
    return !(seconds & 0x80);  // CH bit = 0 means running
}

// Start RTC oscillator
void RTC_start(void) {
    uint8_t seconds;
    
    if (!rtc_initialized) return;
    
    // Read current seconds value
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x00);
    Wire.endTransmission(false);
    Wire.requestFrom(RTC_ADDRESS, 1, true);
    seconds = Wire.read();
    
    // Clear CH bit (bit 7) to start oscillator
    seconds &= 0x7F;
    
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x00);
    Wire.write(seconds);
    Wire.endTransmission(true);
}

// Stop RTC oscillator
void RTC_stop(void) {
    uint8_t seconds;
    
    if (!rtc_initialized) return;
    
    // Read current seconds value
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x00);
    Wire.endTransmission(false);
    Wire.requestFrom(RTC_ADDRESS, 1, true);
    seconds = Wire.read();
    
    // Set CH bit (bit 7) to stop oscillator
    seconds |= 0x80;
    
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x00);
    Wire.write(seconds);
    Wire.endTransmission(true);
}