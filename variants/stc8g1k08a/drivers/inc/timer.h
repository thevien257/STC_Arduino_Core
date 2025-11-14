#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "Arduino.h"

// Timer0 register bit positions
#define TR0_BIT   4  // Timer0 run control bit in TCON
#define TF0_BIT   5  // Timer0 overflow flag bit in TCON
#define T0x12_BIT 7  // Timer0 1T mode bit in AUXR

// Timer1 register bit positions
#define TR1_BIT   6  // Timer1 run control bit in TCON
#define TF1_BIT   7  // Timer1 overflow flag bit in TCON
#define T1x12_BIT 6  // Timer1 1T mode bit in AUXR

/**
 * @brief Initialize Timer0 in Mode 1 (16-bit non-auto-reload)
 * 
 * Configures Timer0 for precision timing with 1T mode (1 count per clock cycle).
 * Stops the timer and prepares it for delay operations.
 */
#define timer0_init() do { \
    CLEAR_BIT(TCON, TR0_BIT); \
    CLEAR_BIT_MASK(TMOD, 0x0F); \
    SET_BIT(TMOD, 0); \
    CLEAR_BIT(TMOD, 2); \
    CLEAR_BIT(TMOD, 3); \
    SET_BIT(AUXR, T0x12_BIT); \
} while(0)

/**
 * @brief Delay for specified microseconds (blocking)
 * @param us Microseconds to delay (1 to 4294967295)
 * 
 * Uses Timer0 in Mode 1 (16-bit) for accurate microsecond delays.
 * Automatically handles delays longer than the 16-bit timer capacity
 * by breaking them into multiple iterations.
 * 
 * Example:
 *   delay_us(100);    // Delay 100 microseconds
 *   delay_us(5000);   // Delay 5 milliseconds
 */
#define delay_us(us) do { \
    uint16_t _reload_value; \
    uint32_t _cycles_needed; \
    uint16_t _cycles_this_iteration; \
    if ((us) != 0) { \
        _cycles_needed = ((us) * (F_CPU / 1000000UL)); \
        timer0_init(); \
        while (_cycles_needed > 0) { \
            if (_cycles_needed > 65535UL) { \
                _cycles_this_iteration = 65535; \
            } else { \
                _cycles_this_iteration = (uint16_t)_cycles_needed; \
            } \
            _reload_value = 65536UL - _cycles_this_iteration; \
            TL0 = LOW_BYTE(_reload_value); \
            TH0 = HIGH_BYTE(_reload_value); \
            CLEAR_BIT(TCON, TF0_BIT); \
            SET_BIT(TCON, TR0_BIT); \
            while (!IS_BIT_SET(TCON, TF0_BIT)); \
            CLEAR_BIT(TCON, TR0_BIT); \
            _cycles_needed -= _cycles_this_iteration; \
        } \
    } \
} while(0)

/**
 * @brief Delay for specified milliseconds (blocking)
 * @param ms Milliseconds to delay (1 to 65535)
 * 
 * Optimized to reduce loop overhead by using larger timer periods.
 * Each millisecond is timed using Timer0 with 1ms reload value.
 * 
 * Example:
 *   delay_ms(100);   // Delay 100 milliseconds
 *   delay_ms(1000);  // Delay 1 second
 */
#define delay_ms(ms) do { \
    uint16_t _reload_value; \
    uint16_t _i; \
    uint32_t _cycles_per_ms; \
    if ((ms) != 0) { \
        _cycles_per_ms = F_CPU / 1000UL; \
        _reload_value = 65536UL - _cycles_per_ms; \
        timer0_init(); \
        for (_i = 0; _i < (ms); _i++) { \
            TL0 = LOW_BYTE(_reload_value); \
            TH0 = HIGH_BYTE(_reload_value); \
            CLEAR_BIT(TCON, TF0_BIT); \
            SET_BIT(TCON, TR0_BIT); \
            while (!IS_BIT_SET(TCON, TF0_BIT)); \
            CLEAR_BIT(TCON, TR0_BIT); \
        } \
    } \
} while(0)

/**
 * @brief Delay for specified seconds (blocking)
 * @param seconds Seconds to delay (1 to 255)
 * 
 * Convenience macro that calls delay_ms() in a loop.
 * For longer delays without blocking, consider using millis() instead.
 * 
 * Example:
 *   delay_s(5);   // Delay 5 seconds
 *   delay_s(60);  // Delay 1 minute
 */
#define delay_s(seconds) do { \
    uint8_t _i; \
    if ((seconds) != 0) { \
        for (_i = 0; _i < (seconds); _i++) { \
            delay_ms(1000); \
        } \
    } \
} while(0)

#endif // TIMER_H