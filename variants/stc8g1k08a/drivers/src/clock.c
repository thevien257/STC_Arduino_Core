#include "Arduino.h"

void clock_init(void)
{
  // CRITICAL: Enable extended RAM/SFR access
  // Without this, all xdata register writes (HIRCCR, CKSEL, etc.) are ignored!
  P_SW2 = 0x80;

  // Enable high-speed IRC
  HIRCCR |= (HIGH << 7);
  while (!(HIRCCR & 0x1))
    ; // Wait for IRC ready
  
  // Clock selection: internal high-precision IRC
  CKSEL &= ~(0x7 << 0);

  // Calculate and set clock divider based on F_CPU
#if F_CPU <= 14700000
  // Use 20MHz band with division for low frequencies
  IRCBAND &= ~0x01; // Select 20MHz band
  CLKDIV = (20000000UL + F_CPU - 1) / F_CPU;

#elif F_CPU <= 26000000
  // Use 20MHz band, possibly with division or direct
  IRCBAND &= ~0x01; // Select 20MHz band
  // For frequencies in this range, IRC should be adjusted via ISP
  // to the target frequency, so typically no division needed
  CLKDIV = 0x01; // No division (MCLK/1)

#elif F_CPU <= 35000000
  // Use 33MHz band
  IRCBAND |= 0x01; // Select 33MHz band
  // IRC should be adjusted via ISP to target frequency
  CLKDIV = 0x01; // No division (MCLK/1)

#else
  // For higher frequencies, use 33MHz band with appropriate settings
  IRCBAND |= 0x01; // Select 33MHz band
  CLKDIV = 0x01; // No division (MCLK/1)
#endif

 P_SW2 = 0x00;  // <-- CRITICAL! You close the XDATA access
}