#include "Arduino.h"

// Forward declare user's interrupt handler
extern void timer0_isr(void) __interrupt(1);
extern void uart1_isr(void) __interrupt(4);

extern void setup(void);
extern void loop(void);

// Initialize system
void init(void)
{
    clock_init();
}

// Main entry point
void main(void)
{
    init();
    setup();

    while (1)
    {
        loop();
    }
}