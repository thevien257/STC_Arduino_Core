#ifndef HARDWARESERIAL_H
#define HARDWARESERIAL_H

#include <stdint.h>
#include "Arduino.h"

// Serial interface structure
typedef struct {
    void (*begin)(uint32_t baud);
    void (*end)(void);
    uint8_t (*available)(void);
    int (*read)(void);
    void (*write)(uint8_t byte);
    void (*print)(const char* str);
    void (*println)(const char* str);
} Serial_t;

// External Serial object
extern Serial_t Serial;

#endif // HARDWARESERIAL_H