#ifndef _VARIANT_STC8G1K08A_H_
#define _VARIANT_STC8G1K08A_H_

#include <stdint.h>
#include "drivers/inc/gpio.h"
#include "drivers/inc/timer.h"
#include "drivers/inc/i2c.h"

// STC8G1K08A Register Definitions
__sfr __at(0x80) P0;
__sfr __at(0x81) SP;
__sfr __at(0x82) DPL;
__sfr __at(0x83) DPH;
__sfr __at(0x87) PCON;
__sfr __at(0x88) TCON;
__sfr __at(0x89) TMOD;
__sfr __at(0x8A) TL0;
__sfr __at(0x8B) TL1;
__sfr __at(0x8C) TH0;
__sfr __at(0x8D) TH1;
__sfr __at(0x8E) AUXR;
__sfr __at(0x8F) INTCLKO; // External interrupt control
__sfr __at(0x90) P1;
__sfr __at(0x91) P1M1;
__sfr __at(0x92) P1M0;
__sfr __at(0x93) P0M1;
__sfr __at(0x94) P0M0;
__sfr __at(0x98) SCON;    // UART1 control register
__sfr __at(0x99) SBUF;    // UART1 data buffer register
__sfr __at(0x9D) IRCBAND; // IRC band selection register
__sfr __at(0x9E) LIRTRIM; // IRC frequency trim register
__sfr __at(0x9F) IRTRIM;  // IRC frequency adjustment register
__sfr __at(0xA0) P2;
__sfr __at(0xA2) P_SW1; // Peripheral port switch register 1
__sfr __at(0xA8) IE;
__sfr __at(0xA9) SADDR; // UART1 slave address register
__sfr __at(0xB0) P3;
__sfr __at(0xB1) P3M1;
__sfr __at(0xB2) P3M0;
__sfr __at(0xB8) IP;
__sfr __at(0xB9) SADEN; // UART1 slave address enable register
__sfr __at(0xBA) P_SW2; // Peripheral port switch register 2
__sfr __at(0xC8) P5;
__sfr __at(0xC9) P5M1;
__sfr __at(0xCA) P5M0;
__sfr __at(0xD0) PSW;
__sfr __at(0xE0) ACC;
__sfr __at(0xEF) AUXINTIF; // Auxiliary interrupt flags
__sfr __at(0xF0) B;

// Extended SFRs (XDATA area) - Clock & Power
__xdata __at(0xFE00) volatile uint8_t CKSEL;
__xdata __at(0xFE01) volatile uint8_t CLKDIV;
__xdata __at(0xFE02) volatile uint8_t HIRCCR;
__xdata __at(0xFE03) volatile uint8_t XOSCCR;
__xdata __at(0xFE04) volatile uint8_t IRC32KCR;
__xdata __at(0xFE05) volatile uint8_t MCLKOCR;

// Extended SFRs - GPIO Pull-up and Input Enable
__xdata __at(0xFE10) volatile uint8_t P0PU;
__xdata __at(0xFE11) volatile uint8_t P1PU;
__xdata __at(0xFE12) volatile uint8_t P2PU;
__xdata __at(0xFE13) volatile uint8_t P3PU;
__xdata __at(0xFE15) volatile uint8_t P5PU;
__xdata __at(0xFE30) volatile uint8_t P0IE;
__xdata __at(0xFE31) volatile uint8_t P1IE;
__xdata __at(0xFE32) volatile uint8_t P2IE;
__xdata __at(0xFE33) volatile uint8_t P3IE;
__xdata __at(0xFE35) volatile uint8_t P5IE;

// Extended SFRs - I2C Registers
__xdata __at(0xFE80) volatile uint8_t I2CCFG;   // I2C Configuration Register
__xdata __at(0xFE81) volatile uint8_t I2CMSCR;  // I2C Master Control Register
__xdata __at(0xFE82) volatile uint8_t I2CMSST;  // I2C Master Status Register
__xdata __at(0xFE83) volatile uint8_t I2CSLCR;  // I2C Slave Control Register
__xdata __at(0xFE84) volatile uint8_t I2CSLST;  // I2C Slave Status Register
__xdata __at(0xFE85) volatile uint8_t I2CSLADR; // I2C Slave Address Register
__xdata __at(0xFE86) volatile uint8_t I2CTXD;   // I2C Data Transmission Register
__xdata __at(0xFE87) volatile uint8_t I2CRXD;   // I2C Data Receive Register
__xdata __at(0xFE88) volatile uint8_t I2CMSAUX; // I2C Master Auxiliary Control Register

// P_SW2 bits
#define EAXFR 0x80 // Extended RAM/SFR access enable

// UART1 pin selection for STC8G1K08A
#define S1_S_P30_P31 0x00 // RxD=P3.0, TxD=P3.1 (default)
#define S1_S_P32_P33 0x40 // RxD=P3.2, TxD=P3.3
#define S1_S_P54_P55 0x80 // RxD=P5.4, TxD=P5.5

// I2C pin selection for STC8G1K08A (bits 5:4 of P_SW2)
#define I2C_S_P32_P33 0x00 // SCL=P3.2, SDA=P3.3 (default)
#define I2C_S_P54_P55 0x10 // SCL=P5.4, SDA=P5.5 (bit 4 = 1)

// I2CCFG Register Bits
#define I2C_ENI2C 0x80  // I2C enable bit
#define I2C_MASTER 0x40 // Master mode (1=Master, 0=Slave)

// I2CMSCR Register - Master Command Bits
#define I2C_CMD_IDLE 0x00     // Standby
#define I2C_CMD_START 0x01    // START command
#define I2C_CMD_SENDDATA 0x02 // Send data command
#define I2C_CMD_RECVACK 0x03  // Receive ACK command
#define I2C_CMD_RECVDATA 0x04 // Receive data command
#define I2C_CMD_SENDACK 0x05  // Send ACK command
#define I2C_CMD_STOP 0x06     // STOP command

// I2CMSST Register Bits
#define I2C_MSBUSY 0x80 // Master busy status
#define I2C_MSIF 0x40   // Master interrupt flag
#define I2C_MSACKI 0x02 // ACK received bit
#define I2C_MSACKO 0x01 // ACK to send bit

#define MASK_TWO_BITS_HIGH 0x03

// UART Mode definitions
#define UART_MODE_1 0x01 // 8-bit UART, variable baud rate

// Port 3 pins
#define P3_0 0
#define P3_1 1
#define P3_2 2
#define P3_3 3

// Port 5 pins
#define P5_4 4
#define P5_5 5

#define PORT3 3
#define PORT5 5

// Interrupt vectors
#define INT0_ISR_VECTOR 0
#define TIMER0_ISR_VECTOR 1
#define INT1_ISR_VECTOR 2
#define UART1_ISR_VECTOR 4
#define INT2_ISR_VECTOR 10
#define INT3_ISR_VECTOR 11
#define INT4_ISR_VECTOR 16
#define I2C_ISR_VECTOR 24 // I2C interrupt vector

#endif