#ifndef Arduino_h
#define Arduino_h

#include <stdint.h>
#include <stdbool.h>
#include "variant.h"

// Arduino Pin Definitions (map to STC8G pins)
#define LED_BUILTIN 13

// Pin modes
#define INPUT           0x00
#define OUTPUT          0x01
#define INPUT_PULLUP    0x02

// Digital values
#define LOW     0
#define HIGH    1

// PinMap structure
typedef struct {
    uint8_t port;
    uint8_t bit;
    uint8_t mask;
} PinMap;

void clock_init(void);

// GPIO functions
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t value);
uint8_t digitalRead(uint8_t pin);

// Timer 0 mode definitions
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);
void delay_s(uint8_t seconds);
void timer0_micros_init(void);
uint32_t micros(void);
uint32_t millis(void);
uint8_t delay_nb(uint32_t *start_time, uint32_t interval_us);

void init(void);

// Setup and loop (Arduino standard)
extern void setup(void);
extern void loop(void);

#endif // Arduino_h