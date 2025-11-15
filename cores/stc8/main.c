#include "Arduino.h"

// Forward declare user's interrupt handler
extern void INT0_ISR(void) __interrupt(INT0_ISR_VECTOR);
extern void INT1_ISR(void) __interrupt(INT1_ISR_VECTOR);
extern void INT2_ISR(void) __interrupt(INT2_ISR_VECTOR);
extern void INT3_ISR(void) __interrupt(INT3_ISR_VECTOR);
extern void INT4_ISR(void) __interrupt(INT4_ISR_VECTOR);
extern void timer0_isr(void) __interrupt(TIMER0_ISR_VECTOR);
extern void uart1_isr(void) __interrupt(UART1_ISR_VECTOR);


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