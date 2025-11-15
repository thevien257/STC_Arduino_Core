// ============= interrupt.h =============
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>
#include "Arduino.h"

// Interrupt modes (Arduino compatible)
// Note: STC8G INT0/INT1 hardware limitations:
// - CHANGE/RISING: Triggers on both rising and falling edges
// - FALLING: Triggers on falling edge only
// - LOW: Not supported (will use FALLING instead)
#define LOW     0
#define CHANGE  1
#define RISING  2
#define FALLING 3

// Interrupt numbers for STC8G1K08A
#define INT0_INTERRUPT  0  // P3.2
#define INT1_INTERRUPT  1  // P3.3
#define INT2_INTERRUPT  2  // P5.4
#define INT3_INTERRUPT  3  // P5.5
#define INT4_INTERRUPT  4  // P3.0

// Map digital pins to interrupt numbers
#define digitalPinToInterrupt(pin) \
    ((pin) == P3_2 ? INT0_INTERRUPT : \
     (pin) == P3_3 ? INT1_INTERRUPT : \
     (pin) == P5_4 ? INT2_INTERRUPT : \
     (pin) == P5_5 ? INT3_INTERRUPT : \
     (pin) == P3_0 ? INT4_INTERRUPT : \
     -1)

// Function pointer type for ISR
typedef void (*voidFuncPtr)(void);

// API functions
void attachInterrupt(uint8_t interrupt, voidFuncPtr userFunc, uint8_t mode);
void detachInterrupt(uint8_t interrupt);

#endif // INTERRUPT_H