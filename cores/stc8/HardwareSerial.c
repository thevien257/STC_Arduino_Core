#include "HardwareSerial.h"
#include "variant.h"

// Circular buffers for RX and TX
#define SERIAL_RX_BUFFER_SIZE 64
#define SERIAL_TX_BUFFER_SIZE 64

// Move buffers to XRAM to save internal RAM
static volatile __xdata uint8_t rx_buffer[SERIAL_RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

static volatile __xdata uint8_t tx_buffer[SERIAL_TX_BUFFER_SIZE];
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;

#ifndef UART1_PIN_SELECT
#define UART1_PIN_SELECT S1_S_P30_P31 // Default: P3.0/P3.1
#endif

static volatile uint8_t tx_transmitting = 0;

void uart1_isr(void) __interrupt(4)
{
  // Handle receive interrupt
  if (READ_BIT(SCON, 0)) // RI flag
  {
    uint8_t next_head = (rx_head + 1) % SERIAL_RX_BUFFER_SIZE;
    uint8_t received_byte = SBUF;

    if (next_head != rx_tail)
    {
      rx_buffer[rx_head] = received_byte;
      rx_head = next_head;
    }

    CLEAR_BIT(SCON, 0); // Clear RI
  }

  // Handle transmit interrupt
  if (READ_BIT(SCON, 1)) // TI flag
  {
    CLEAR_BIT(SCON, 1); // Clear TI immediately
    
    // Check if more data to send
    if (tx_head != tx_tail)
    {
      // Send next byte
      SBUF = tx_buffer[tx_tail];
      tx_tail = (tx_tail + 1) % SERIAL_TX_BUFFER_SIZE;
    }
    else
    {
      // Nothing more to send
      tx_transmitting = 0;
    }
  }
}

// Calculate Timer1 reload value for baud rate using Mode 0 with 1T
// Formula: reload = 65536 - (SYSclk / (4 * baudrate))
static uint16_t calculate_timer1_reload(uint32_t baud)
{
  uint32_t reload;
  reload = 65536UL - (F_CPU / (4UL * baud));
  return (uint16_t)reload;
}

static void serial_begin(uint32_t baud)
{
  uint16_t reload;

  // Configure UART1 pin switching
  P_SW1 = (P_SW1 & 0x3F) | UART1_PIN_SELECT;

#if UART1_PIN_SELECT == S1_S_P30_P31
  // Default pins - no extra config needed
#elif UART1_PIN_SELECT == S1_S_P32_P33
  SET_BIT(P_SW2, 7);
  CLEAR_BIT(P3M1, 2);
  CLEAR_BIT(P3M0, 2);
  CLEAR_BIT(P3M1, 3);
  CLEAR_BIT(P3M0, 3);
  CLEAR_BIT(P_SW2, 7);
#elif UART1_PIN_SELECT == S1_S_P54_P55
  SET_BIT(P_SW2, 7);
  CLEAR_BIT(P5M1, 4);
  CLEAR_BIT(P5M0, 4);
  CLEAR_BIT(P5M1, 5);
  CLEAR_BIT(P5M0, 5);
  CLEAR_BIT(P_SW2, 7);
#endif

  // Configure UART1 for Mode 1 (8-bit UART, variable baud rate)
  SCON = 0x50;

  // Disable Timer1 while configuring
  CLEAR_BIT(TCON, 6); // TR1 = 0
  
  // Configure Timer1 for baud rate generation
  // Set S1ST2 = 0 to use Timer1 as baud rate generator
  CLEAR_BIT(AUXR, 0); // S1ST2 = 0
  
  // Set T1x12 = 1 for 1T mode (SYSclk/1)
  SET_BIT(AUXR, 6); // T1x12 = 1
  
  // Configure Timer1 Mode 0 (16-bit auto-reload mode)
  // TMOD[5:4] = 00 for Mode 0
  CLEAR_BIT_MASK(TMOD, 0xF0); // Clear upper nibble (Timer1 bits)
  CLEAR_BIT(TMOD, 7); // GATE = 0
  CLEAR_BIT(TMOD, 6); // C/T = 0 (timer mode)
  CLEAR_BIT(TMOD, 5); // M1 = 0
  CLEAR_BIT(TMOD, 4); // M0 = 0  -> Mode 0

  // Calculate reload value
  reload = calculate_timer1_reload(baud);
  
  // Set reload value (write when TR1=0 sets both TH1/TL1 and RL_TH1/RL_TL1)
  TH1 = (uint8_t)(reload >> 8);   // High byte
  TL1 = (uint8_t)(reload & 0xFF); // Low byte

  // Clear Timer1 overflow flag
  CLEAR_BIT(TCON, 7); // TF1 = 0

  // Start Timer1
  SET_BIT(TCON, 6); // TR1 = 1

  // Reset buffers
  rx_head = 0;
  rx_tail = 0;
  tx_head = 0;
  tx_tail = 0;
  tx_transmitting = 0;

  // Enable UART interrupt
  SET_BIT(IE, 4); // ES = 1
  SET_BIT(IE, 7); // EA = 1
}

static void serial_end(void)
{
  CLEAR_BIT(IE, 4);
  CLEAR_BIT(TCON, 6);
  CLEAR_BIT(SCON, 4);
}

static uint8_t serial_available(void)
{
  return (SERIAL_RX_BUFFER_SIZE + rx_head - rx_tail) % SERIAL_RX_BUFFER_SIZE;
}

static int serial_read(void)
{
  if (rx_head == rx_tail)
  {
    return -1;
  }

  int data = rx_buffer[rx_tail];
  rx_tail = (rx_tail + 1) % SERIAL_RX_BUFFER_SIZE;
  return data;
}

static void serial_write(uint8_t byte)
{
  uint8_t next_head = (tx_head + 1) % SERIAL_TX_BUFFER_SIZE;
  
  // Wait if buffer is full
  while (next_head == tx_tail);
  
  // Disable interrupts for atomic operation
  CLEAR_BIT(IE, 4);
  
  // Add byte to buffer
  tx_buffer[tx_head] = byte;
  tx_head = next_head;
  
  // Start transmission if not already transmitting
  if (!tx_transmitting)
  {
    tx_transmitting = 1;
    SBUF = tx_buffer[tx_tail];
    tx_tail = (tx_tail + 1) % SERIAL_TX_BUFFER_SIZE;
  }
  
  SET_BIT(IE, 4); // Re-enable interrupts
}

static void serial_print(const char *str)
{
  while (*str)
  {
    serial_write(*str++);
  }
}

static void serial_println(const char *str)
{
  serial_print(str);
  serial_write('\r');
  serial_write('\n');
}

// Serial object instance
Serial_t Serial = {
    .begin = serial_begin,
    .end = serial_end,
    .available = serial_available,
    .read = serial_read,
    .write = serial_write,
    .print = serial_print,
    .println = serial_println,
};