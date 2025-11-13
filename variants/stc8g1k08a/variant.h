#ifndef _VARIANT_STC8G1K08A_H_
#define _VARIANT_STC8G1K08A_H_

#include <stdint.h>
#include "drivers/inc/gpio.h"

// STC8G1K08A Register Definitions
__sfr __at (0x80) P0;
__sfr __at (0x81) SP;
__sfr __at (0x82) DPL;
__sfr __at (0x83) DPH;
__sfr __at (0x87) PCON;
__sfr __at (0x88) TCON;
__sfr __at (0x89) TMOD;
__sfr __at (0x8A) TL0;
__sfr __at (0x8B) TL1;
__sfr __at (0x8C) TH0;
__sfr __at (0x8D) TH1;
__sfr __at (0x8E) AUXR;
__sfr __at (0x90) P1;
__sfr __at (0x91) P1M1;
__sfr __at (0x92) P1M0;
__sfr __at (0x93) P0M1;
__sfr __at (0x94) P0M0;
__sfr __at (0x98) SCON;
__sfr __at (0x99) SBUF;
__sfr __at (0x9D) IRCBAND;    // IRC band selection register
__sfr __at (0x9E) LIRTRIM;    // IRC frequency trim register
__sfr __at (0x9F) IRTRIM;     // IRC frequency adjustment register
__sfr __at (0xA0) P2;
__sfr __at (0xA8) IE;
__sfr __at (0xB0) P3;
__sfr __at (0xB1) P3M1;
__sfr __at (0xB2) P3M0;
__sfr __at (0xB8) IP;
__sfr __at (0xBA) P_SW2;  // Added for EAXFR access
__sfr __at (0xC8) P5;
__sfr __at (0xC9) P5M1;
__sfr __at (0xCA) P5M0;
__sfr __at (0xD0) PSW;
__sfr __at (0xE0) ACC;
__sfr __at (0xF0) B;

// Extended SFRs (XDATA area) - accessed via MOVX instructions when P_SW2.7=1
__xdata __at (0xFE00) volatile uint8_t CKSEL;    // Clock selection register
__xdata __at (0xFE01) volatile uint8_t CLKDIV;   // Clock divider register
__xdata __at (0xFE02) volatile uint8_t HIRCCR;   // High-speed IRC control register
__xdata __at (0xFE03) volatile uint8_t XOSCCR;   // External oscillator control register
__xdata __at (0xFE04) volatile uint8_t IRC32KCR; // 32KHz IRC control register
__xdata __at (0xFE05) volatile uint8_t MCLKOCR;  // Master clock output control register

__xdata __at (0xFE13) volatile uint8_t P3PU;
__xdata __at (0xFE15) volatile uint8_t P5PU;
__xdata __at (0xFE33) volatile uint8_t P3IE;
__xdata __at (0xFE35) volatile uint8_t P5IE;

// P_SW2 bits
#define EAXFR 0x80  // Bit 7: Enable access to extended SFRs

// Port 3 pins (P3.0-P3.7 = pins 16-23)
#define P3_0 16
#define P3_1 17
#define P3_2 18
#define P3_3 19

// Port 5 pins (P5.0-P5.5 = pins 24-29)
#define P5_4 28
#define P5_5 29

#endif