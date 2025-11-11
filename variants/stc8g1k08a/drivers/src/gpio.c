#include "Arduino.h"

// Map pin numbers to actual physical ports
static void map_pin(uint8_t pin, PinMap *p)
{
    if (pin >= 16 && pin <= 23) {
        p->port = 3;  // P3
        p->bit = pin - 16;
    } else if (pin >= 24 && pin <= 29) {
        p->port = 5;  // P5
        p->bit = pin - 24;
    } else if (pin >= 8 && pin <= 15) {
        p->port = 1;  // P1
        p->bit = pin - 8;
    } else {
        p->port = 0;  // P0
        p->bit = pin;
    }
    p->mask = 1 << p->bit;
}

void pinMode(uint8_t pin, uint8_t mode)
{
    PinMap p;
    map_pin(pin, &p);
    
    P_SW2 = 0x80;
    __asm__("nop");
    
    switch (p.port)
    {
    case 0: // P0
        if (mode == INPUT_PULLUP) {
            P0M1 |= p.mask;
            P0M0 &= ~p.mask;
        } else if (mode == INPUT) {
            P0M1 |= p.mask;
            P0M0 &= ~p.mask;
        } else if (mode == OUTPUT) {
            P0M1 &= ~p.mask;
            P0M0 |= p.mask;
        }
        break;
        
    case 1: // P1
        if (mode == INPUT_PULLUP) {
            P1M1 |= p.mask;
            P1M0 &= ~p.mask;
        } else if (mode == INPUT) {
            P1M1 |= p.mask;
            P1M0 &= ~p.mask;
        } else if (mode == OUTPUT) {
            P1M1 &= ~p.mask;
            P1M0 |= p.mask;
        }
        break;
    
    case 3: // P3
        P3IE |= p.mask;
        if (mode == INPUT_PULLUP) {
            P3M1 |= p.mask;
            P3M0 &= ~p.mask;
            P3PU |= p.mask;
        } else if (mode == INPUT) {
            P3M1 |= p.mask;
            P3M0 &= ~p.mask;
            P3PU &= ~p.mask;
        } else if (mode == OUTPUT) {
            P3M1 &= ~p.mask;
            P3M0 |= p.mask;
            P3PU &= ~p.mask;
        }
        break;
        
    case 5: // P5
        P5IE |= p.mask;
        if (mode == INPUT_PULLUP) {
            P5M1 |= p.mask;
            P5M0 &= ~p.mask;
            P5PU |= p.mask;
        } else if (mode == INPUT) {
            P5M1 |= p.mask;
            P5M0 &= ~p.mask;
            P5PU &= ~p.mask;
        } else if (mode == OUTPUT) {
            P5M1 &= ~p.mask;
            P5M0 |= p.mask;
            P5PU &= ~p.mask;
        }
        break;
    }
    
    __asm__("nop");
    P_SW2 = 0x00;
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    PinMap p;
    map_pin(pin, &p);
    
    switch (p.port)
    {
    case 0:
        if (value) P0 |= p.mask;
        else P0 &= ~p.mask;
        break;
    case 1:
        if (value) P1 |= p.mask;
        else P1 &= ~p.mask;
        break;
    case 3:
        if (value) P3 |= p.mask;
        else P3 &= ~p.mask;
        break;
    case 5:
        if (value) P5 |= p.mask;
        else P5 &= ~p.mask;
        break;
    }
}

uint8_t digitalRead(uint8_t pin)
{
    PinMap p;
    volatile uint8_t pin_state;
    
    map_pin(pin, &p);
    
    switch (p.port)
    {
    case 0:
        pin_state = P0;
        return (pin_state & p.mask) ? HIGH : LOW;
    case 1:
        pin_state = P1;
        return (pin_state & p.mask) ? HIGH : LOW;
    case 3:
        pin_state = P3;
        return (pin_state & p.mask) ? HIGH : LOW;
    case 5:
        pin_state = P5;
        return (pin_state & p.mask) ? HIGH : LOW;
    }
    
    return LOW;
}