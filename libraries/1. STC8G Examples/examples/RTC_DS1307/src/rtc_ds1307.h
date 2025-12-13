#ifndef SIMPLE_RTC_H
#define SIMPLE_RTC_H

#include <stdint.h>
#include <stdbool.h>
#include "Arduino.h"

// RTC I2C address
#define RTC_ADDRESS 0x68

// Day of week constants
#define RTC_SUNDAY    1
#define RTC_MONDAY    2
#define RTC_TUESDAY   3
#define RTC_WEDNESDAY 4
#define RTC_THURSDAY  5
#define RTC_FRIDAY    6
#define RTC_SATURDAY  7

// Simple RTC functions
bool RTC_begin(I2cPinSelect_t pins);
bool RTC_setTime(uint8_t year, uint8_t month, uint8_t date, 
                 uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day);
bool RTC_getTime(uint8_t *year, uint8_t *month, uint8_t *date, 
                 uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *day);
bool RTC_isRunning(void);
void RTC_start(void);
void RTC_stop(void);

// Helper functions
uint8_t bcd_to_dec(uint8_t bcd);
uint8_t dec_to_bcd(uint8_t dec);

#endif // SIMPLE_RTC_H