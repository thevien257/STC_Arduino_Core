#include "Arduino.h"

/**
 * @brief Delay for specified microseconds (blocking)
 * @param us Microseconds to delay (1 to 65535)
 *
 * Uses Timer0 in Mode 1 (16-bit) for accurate microsecond delays.
 */
void delay_us(uint16_t us)
{
  uint16_t reload_value;
  
  // Stop timer0
  TCON &= ~(1 << 4); // TR0 = 0

  // Configure Timer0: Mode 1 (16-bit non-auto-reload) 
  TMOD &= 0xF0;      // Clear T0 mode bits
  TMOD |= 0x01;      // Mode 1: 16-bit
  TMOD &= ~(1 << 2); // C/T = 0 (timer mode)
  TMOD &= ~(1 << 3); // GATE = 0

  // Use 1T mode for precision (1 count per clock cycle)
  AUXR |= (1 << 7); // T0x12 = 1 (1T mode)

  // Calculate cycles needed for requested microseconds
  // cycles = (us * F_CPU) / 1000000
  uint32_t cycles = ((uint32_t)us * (F_CPU / 1000000UL));
  
  // Timer counts up from reload_value to 65535, then sets TF0
  // So: reload_value = 65536 - cycles
  reload_value = 65536UL - cycles;

  // Load timer registers
  TL0 = (uint8_t)(reload_value & 0xFF);
  TH0 = (uint8_t)(reload_value >> 8);

  // Clear overflow flag
  TCON &= ~(1 << 5); // TF0 = 0
  
  // Start timer
  TCON |= (1 << 4);  // TR0 = 1

  // Wait for overflow
  while (!(TCON & (1 << 5)))
    ;

  // Stop timer
  TCON &= ~(1 << 4); // TR0 = 0
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
  
  if (ms == 0) return;

  // Stop timer0
  TCON &= ~(1 << 4); // TR0 = 0

  // Configure Timer0: Mode 1 (16-bit)
  TMOD &= 0xF0;      // Clear T0 mode bits
  TMOD |= 0x01;      // Mode 1
  TMOD &= ~(1 << 2); // Timer mode
  TMOD &= ~(1 << 3); // GATE = 0

  // Use 1T mode
  AUXR |= (1 << 7);

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
    TCON |= (1 << 4);  // TR0 = 1
    
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

// ============================================================================
// INTERRUPT-BASED MICROSECOND COUNTER (Advanced - optional)
// ============================================================================

static volatile uint32_t timer0_micros_counter = 0;
static volatile uint8_t timer0_initialized = 0;

/**
 * @brief Initialize Timer0 for interrupt-based microsecond counter
 *
 * Generates interrupt every 1000us (1ms) for continuously running micros() counter.
 */
void timer0_micros_init(void)
{
  uint16_t reload_value;
  
  if (timer0_initialized) return; // Already initialized
  
  // Stop timer0
  TCON &= ~(1 << 4); // TR0 = 0

  // Configure Timer0: Mode 1 (16-bit)
  TMOD &= 0xF0;
  TMOD |= 0x01;
  TMOD &= ~(1 << 2); // Timer mode
  TMOD &= ~(1 << 3); // GATE = 0

  // Use 1T mode
  AUXR |= (1 << 7);

  // Configure for 1000us (1ms) interrupts
  uint32_t cycles = F_CPU / 1000UL;
  reload_value = 65536UL - cycles;
  
  TL0 = (uint8_t)(reload_value & 0xFF);
  TH0 = (uint8_t)(reload_value >> 8);

  // Clear overflow flag
  TCON &= ~(1 << 5); // TF0 = 0

  // Enable Timer0 interrupt
  IE |= (1 << 1); // ET0 = 1
  IE |= (1 << 7); // EA = 1

  // Reset counter
  timer0_micros_counter = 0;
  timer0_initialized = 1;

  // Start timer
  TCON |= (1 << 4); // TR0 = 1
}

/**
 * @brief Timer0 interrupt handler for microsecond counter
 *
 * Called every 1000us (1ms) - must reload timer in Mode 1
 */
void timer0_isr(void) __interrupt(1)
{
  uint16_t reload_value;
  
  // Reload timer for next 1ms (Mode 1 doesn't auto-reload)
  uint32_t cycles = F_CPU / 1000UL;
  reload_value = 65536UL - cycles;
  TL0 = (uint8_t)(reload_value & 0xFF);
  TH0 = (uint8_t)(reload_value >> 8);
  
  // Increment microsecond counter by 1000
  timer0_micros_counter += 1000;
}

/**
 * @brief Get microseconds since timer0_micros_init() was called
 * @return Microsecond count (wraps after ~71 minutes)
 */
uint32_t micros(void)
{
  uint32_t m;
  uint16_t timer_val;
  uint8_t old_ea;

  if (!timer0_initialized) {
    timer0_micros_init();
  }

  old_ea = IE & 0x80; // Save EA state
  IE &= ~0x80;        // Disable interrupts

  m = timer0_micros_counter;
  
  // Read current timer value for sub-millisecond precision
  timer_val = ((uint16_t)TH0 << 8) | TL0;
  
  // Calculate elapsed cycles in current millisecond
  uint32_t cycles_per_ms = F_CPU / 1000UL;
  uint16_t reload_value = 65536UL - cycles_per_ms;
  
  // Timer counts up from reload_value to 65535
  if (timer_val >= reload_value) {
    uint32_t elapsed_cycles = timer_val - reload_value;
    // Convert to microseconds: (elapsed / cycles_per_ms) * 1000
    uint32_t partial_us = (elapsed_cycles * 1000UL) / cycles_per_ms;
    m += partial_us;
  }

  IE |= old_ea; // Restore EA state

  return m;
}

/**
 * @brief Get milliseconds since timer0_micros_init() was called
 * @return Millisecond count
 */
uint32_t millis(void)
{
  return micros() / 1000;
}

/**
 * @brief Non-blocking delay check
 * @param start_time Pointer to store start time
 * @param interval_us Interval in microseconds
 * @return 1 if interval elapsed, 0 if still waiting
 *
 * Example usage:
 *   static uint32_t last_time = 0;
 *   if (delay_nb(&last_time, 1000000)) {  // Every 1 second
 *       // Do something
 *   }
 */
uint8_t delay_nb(uint32_t *start_time, uint32_t interval_us)
{
  uint32_t current = micros();
  
  if (current - *start_time >= interval_us)
  {
    *start_time = current;
    return 1;
  }
  return 0;
}