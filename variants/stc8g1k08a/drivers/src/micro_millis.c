#include "Arduino.h"

static volatile uint8_t timer_initialized = 0;
static volatile uint32_t _millis = 0;
static volatile uint32_t _micros_overflow = 0;

// Store timer configuration for micros calculation
static uint16_t timer_reload_value = 0;
static uint16_t cycles_per_ms = 0;

static void timer_init(void)
{
  // Stop timer first
  TCON &= ~(1 << 4);  // TR0 = 0

  // Configure Timer0: Mode 0 (13-bit auto-reload)
  TMOD &= 0xF0;       // Clear T0 mode bits (M1=0, M0=0 = Mode 0)
  TMOD &= ~(1 << 2);  // C/T = 0 (timer mode)
  TMOD &= ~(1 << 3);  // GATE = 0

  // Use 1T mode for precision
  AUXR |= (1 << 7);   // T0x12 = 1 (1T mode)

  // Calculate reload value for 1ms at F_CPU in 1T mode
  // For 20MHz: 65536 - 20000 = 45536 = 0xB1E0
  cycles_per_ms = F_CPU / 1000UL;
  timer_reload_value = 65536UL - cycles_per_ms;
  
  TH0 = (uint8_t)(timer_reload_value >> 8);
  TL0 = (uint8_t)(timer_reload_value & 0xFF);

  // Clear overflow flag
  TCON &= ~(1 << 5);  // TF0 = 0

  // Enable Timer0 interrupt
  IE |= (1 << 1);     // ET0 = 1

  // Enable global interrupts
  IE |= (1 << 7);     // EA = 1

  // Start timer
  TCON |= (1 << 4);   // TR0 = 1
  
  timer_initialized = 1;
}

uint32_t millis(void)
{
  uint32_t m;
  
  // Initialize on first call
  if (!timer_initialized)
  {
    timer_init();
  }
  
  // Disable interrupts briefly to read atomically
  IE &= ~(1 << 7);  // EA = 0
  m = _millis;
  IE |= (1 << 7);   // EA = 1
  
  return m;
}

uint32_t micros(void)
{
  uint32_t m;
  uint16_t timer_count;
  uint8_t tl, th;
  
  // Initialize on first call
  if (!timer_initialized)
  {
    timer_init();
  }
  
  // Disable interrupts to read atomically
  IE &= ~(1 << 7);  // EA = 0
  
  // Read current timer value
  tl = TL0;
  th = TH0;
  timer_count = ((uint16_t)th << 8) | tl;
  
  // Get current millisecond count
  m = _millis;
  
  // Re-enable interrupts
  IE |= (1 << 7);   // EA = 1
  
  // Calculate elapsed cycles since last reload
  uint16_t elapsed_cycles;
  if (timer_count >= timer_reload_value) {
    elapsed_cycles = timer_count - timer_reload_value;
  } else {
    // Handle overflow case (shouldn't happen often with proper timing)
    elapsed_cycles = (65536UL - timer_reload_value) + timer_count;
  }
  
  // Convert to microseconds
  // elapsed_us = (elapsed_cycles * 1000000) / F_CPU
  // Simplified: elapsed_us = (elapsed_cycles * 1000) / cycles_per_ms
  uint32_t elapsed_us = ((uint32_t)elapsed_cycles * 1000UL) / cycles_per_ms;
  
  // Return total microseconds
  return (m * 1000UL) + elapsed_us;
}

// Timer 0 Overflow Interrupt Service Routine
void timer0_isr(void) __interrupt(1)
{
  _millis++;
  // Mode 0 automatically reloads from hidden registers
  // TF0 is automatically cleared when entering ISR
}