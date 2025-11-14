#ifndef HARDWARESERIAL_H
#define HARDWARESERIAL_H

#include <stdint.h>
#include "Arduino.h"

// Pin selection constants
typedef enum
{
    UART_PINS_DEFAULT = 0, // P3.0/P3.1
    UART_PINS_P32_P33 = 1,    // P3.2/P3.3
    UART_PINS_P54_P55 = 2     // P5.4/P5.5
} UartPinSelect_t;

// Serial interface structure
typedef struct
{
    void (*begin)(uint32_t baud);
    void (*beginWithPins)(uint32_t baud, UartPinSelect_t pins) __reentrant;
    void (*end)(void);
    uint8_t (*available)(void);
    int (*read)(void);
    void (*write)(uint8_t byte);
    void (*print)(const char *str);
    void (*println)(const char *str);
} Serial_t;

// External Serial object
extern Serial_t Serial;

#endif // HARDWARESERIAL_H