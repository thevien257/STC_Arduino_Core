// ============= interrupt.c =============
#include "interrupt.h"
#include "variant.h"

// Storage for user interrupt handlers
static voidFuncPtr int0_user_handler = 0;
static voidFuncPtr int1_user_handler = 0;
static voidFuncPtr int2_user_handler = 0;
static voidFuncPtr int3_user_handler = 0;
static voidFuncPtr int4_user_handler = 0;

void attachInterrupt(uint8_t interrupt, voidFuncPtr userFunc, uint8_t mode)
{
    if (!userFunc) return;
    
    switch (interrupt)
    {
        case INT0_INTERRUPT: // P3.2
            int0_user_handler = userFunc;
            
            // Configure trigger mode
            // IT0 = 0: triggered by both rising and falling edges (CHANGE mode)
            // IT0 = 1: triggered only by falling edge (FALLING mode)
            if (mode == CHANGE || mode == RISING) {
                CLEAR_BIT(TCON, 0); // IT0 = 0 (both edges)
            } else { // FALLING or LOW
                SET_BIT(TCON, 0);   // IT0 = 1 (falling edge only)
            }
            
            // Clear flag and enable interrupt
            CLEAR_BIT(TCON, 1);     // Clear IE0
            SET_BIT(IE, 0);         // EX0 = 1 (enable INT0)
            SET_BIT(IE, 7);         // EA = 1 (global enable)
            break;
            
        case INT1_INTERRUPT: // P3.3
            int1_user_handler = userFunc;
            
            // Configure trigger mode
            // IT1 = 0: triggered by both rising and falling edges (CHANGE mode)
            // IT1 = 1: triggered only by falling edge (FALLING mode)
            if (mode == CHANGE || mode == RISING) {
                CLEAR_BIT(TCON, 2); // IT1 = 0 (both edges)
            } else { // FALLING or LOW
                SET_BIT(TCON, 2);   // IT1 = 1 (falling edge only)
            }
            
            // Clear flag and enable interrupt
            CLEAR_BIT(TCON, 3);     // Clear IE1
            SET_BIT(IE, 2);         // EX1 = 1 (enable INT1)
            SET_BIT(IE, 7);         // EA = 1 (global enable)
            break;
            
        case INT2_INTERRUPT: // P5.4
            int2_user_handler = userFunc;
            
            // INT2 only supports edge-triggered (falling edge by default)
            // AUXINTIF &= ~0x20;      // Clear INT2IF
            SET_BIT(INTCLKO, 4);    // EX2 = 1 (enable INT2)
            SET_BIT(IE, 7);         // EA = 1 (global enable)
            break;
            
        case INT3_INTERRUPT: // P5.5
            int3_user_handler = userFunc;
            
            // INT3 only supports edge-triggered (falling edge by default)
            // AUXINTIF &= ~0x40;      // Clear INT3IF
            SET_BIT(INTCLKO, 5);    // EX3 = 1 (enable INT3)
            SET_BIT(IE, 7);         // EA = 1 (global enable)
            break;
            
        case INT4_INTERRUPT: // P3.0
            int4_user_handler = userFunc;
            
            // INT4 only supports edge-triggered (falling edge by default)
            // AUXINTIF &= ~0x80;      // Clear INT4IF
            SET_BIT(INTCLKO, 6);    // EX4 = 1 (enable INT4)
            SET_BIT(IE, 7);         // EA = 1 (global enable)
            break;
    }
}

void detachInterrupt(uint8_t interrupt)
{
    switch (interrupt)
    {
        case INT0_INTERRUPT:
            CLEAR_BIT(IE, 0);       // EX0 = 0 (disable INT0)
            int0_user_handler = 0;
            break;
            
        case INT1_INTERRUPT:
            CLEAR_BIT(IE, 2);       // EX1 = 0 (disable INT1)
            int1_user_handler = 0;
            break;
            
        case INT2_INTERRUPT:
            CLEAR_BIT(INTCLKO, 4);  // EX2 = 0 (disable INT2)
            int2_user_handler = 0;
            break;
            
        case INT3_INTERRUPT:
            CLEAR_BIT(INTCLKO, 5);  // EX3 = 0 (disable INT3)
            int3_user_handler = 0;
            break;
            
        case INT4_INTERRUPT:
            CLEAR_BIT(INTCLKO, 6);  // EX4 = 0 (disable INT4)
            int4_user_handler = 0;
            break;
    }
}

// Interrupt Service Routines
void INT0_ISR(void) __interrupt(0)
{
    if (int0_user_handler) {
        int0_user_handler();
    }
    CLEAR_BIT(TCON, 1); // Clear IE0 flag
}

void INT1_ISR(void) __interrupt(2)
{
    if (int1_user_handler) {
        int1_user_handler();
    }
    CLEAR_BIT(TCON, 3); // Clear IE1 flag
}

void INT2_ISR(void) __interrupt(10)
{
    if (int2_user_handler) {
        int2_user_handler();
    }
    AUXINTIF &= ~0x20; // Clear INT2IF flag
}

void INT3_ISR(void) __interrupt(11)
{
    if (int3_user_handler) {
        int3_user_handler();
    }
    AUXINTIF &= ~0x40; // Clear INT3IF flag
}

void INT4_ISR(void) __interrupt(16)
{
    if (int4_user_handler) {
        int4_user_handler();
    }
    AUXINTIF &= ~0x80; // Clear INT4IF flag
}