#include "Arduino.h"

void timer0_init()
{
  // Stop timer0
  TCON &= ~(1 << 4); // TR0 = 0

  // Configure Timer0: Mode 1 (16-bit non-auto-reload)
  TMOD &= 0xF0;      // Clear T0 mode bits
  TMOD |= 0x01;      // Mode 1: 16-bit
  TMOD &= ~(1 << 2); // C/T = 0 (timer mode)
  TMOD &= ~(1 << 3); // GATE = 0

  // Use 1T mode for precision (1 count per clock cycle)
  AUXR |= (1 << 7); // T0x12 = 1 (1T mode)
}

/**
 * @brief Delay for specified microseconds (blocking)
 * @param us Microseconds to delay (1 to 65535)
 *
 * Uses Timer0 in Mode 1 (16-bit) for accurate microsecond delays.
 */
void delay_us(uint32_t us)
{
  uint16_t reload_value;
  uint32_t cycles_needed;

  if (us == 0)
    return;

  timer0_init();

  // Calculate total cycles needed
  cycles_needed = (us * (F_CPU / 1000000UL));

  // Break into chunks if delay is too large for 16-bit timer
  while (cycles_needed > 0)
  {
    uint16_t cycles_this_iteration;

    // Maximum cycles per iteration is 65535
    if (cycles_needed > 65535UL)
    {
      cycles_this_iteration = 65535;
    }
    else
    {
      cycles_this_iteration = (uint16_t)cycles_needed;
    }

    // Timer counts up from reload_value to 65535, then sets TF0
    reload_value = 65536UL - cycles_this_iteration;

    // Load timer
    TL0 = (uint8_t)(reload_value & 0xFF);
    TH0 = (uint8_t)(reload_value >> 8);

    // Clear overflow flag
    TCON &= ~(1 << 5); // TF0 = 0

    // Start timer
    TCON |= (1 << 4); // TR0 = 1

    // Wait for overflow
    while (!(TCON & (1 << 5)))
      ;

    // Stop timer
    TCON &= ~(1 << 4); // TR0 = 0

    // Subtract completed cycles
    cycles_needed -= cycles_this_iteration;
  }
}

/**
 * @brief Delay for specified milliseconds (blocking)
 * @param ms Milliseconds to delay
 *
 * Optimized to reduce loop overhead by using larger timer periods.
 */
void delay_ms(uint16_t ms)
{
  uint16_t reload_value;
  uint16_t i;

  if (ms == 0)
    return;

  timer0_init();

  // Calculate reload value for 1ms
  // At F_CPU Hz, 1ms = F_CPU/1000 cycles
  uint32_t cycles_per_ms = F_CPU / 1000UL;
  reload_value = 65536UL - cycles_per_ms;

  // Delay ms times
  for (i = 0; i < ms; i++)
  {
    // Load timer for 1ms
    TL0 = (uint8_t)(reload_value & 0xFF);
    TH0 = (uint8_t)(reload_value >> 8);

    // Clear overflow flag
    TCON &= ~(1 << 5); // TF0 = 0

    // Start timer
    TCON |= (1 << 4); // TR0 = 1

    // Wait for overflow
    while (!(TCON & (1 << 5)))
      ;

    // Stop timer
    TCON &= ~(1 << 4); // TR0 = 0
  }
}

/**
 * @brief Delay for specified seconds (blocking)
 * @param seconds Seconds to delay (1 to 255)
 */
void delay_s(uint8_t seconds)
{
  uint8_t i;

  for (i = 0; i < seconds; i++)
  {
    delay_ms(1000);
  }
}