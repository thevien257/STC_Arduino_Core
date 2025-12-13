#include "HardwareSerial.h"
#include "variant.h"

// Circular buffer for RX only
#define SERIAL_RX_BUFFER_SIZE 64

// Move buffer to XRAM to save internal RAM
static volatile __xdata uint8_t rx_buffer[SERIAL_RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

// Move to XRAM to save internal RAM
static __xdata UartPinSelect_t current_pins = UART_PINS_DEFAULT;

#define SERIAL_LINE_BUFFER_SIZE 64
static __xdata char serial_line_buffer[SERIAL_LINE_BUFFER_SIZE];

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
}

// Calculate Timer1 reload value for baud rate using Mode 0 with 1T
// Formula: reload = 65536 - (SYSclk / (4 * baudrate))
static uint16_t calculate_timer1_reload(uint32_t baud)
{
  uint32_t reload;
  reload = 65536UL - (F_CPU / (4UL * baud));
  return (uint16_t)reload;
}

static void serial_begin_with_pins(uint32_t baud, UartPinSelect_t pins) __reentrant
{
  uint16_t reload;
  current_pins = pins;

  // Configure UART1 pin switching based on enum
  switch (pins)
  {
  case UART_PINS_DEFAULT: // P3.0/P3.1
    P_SW1 = (P_SW1 & 0x3F) | S1_S_P30_P31;
    break;

  case UART_PINS_P32_P33: // P3.2/P3.3
    P_SW1 = (P_SW1 & 0x3F) | S1_S_P32_P33;
    SET_BIT(P_SW2, 7);
    CLEAR_BIT(P3M1, 2);
    CLEAR_BIT(P3M0, 2);
    CLEAR_BIT(P3M1, 3);
    CLEAR_BIT(P3M0, 3);
    CLEAR_BIT(P_SW2, 7);
    break;

  case UART_PINS_P54_P55: // P5.4/P5.5
    P_SW1 = (P_SW1 & 0x3F) | S1_S_P54_P55;
    SET_BIT(P_SW2, 7);
    CLEAR_BIT(P5M1, 4);
    CLEAR_BIT(P5M0, 4);
    CLEAR_BIT(P5M1, 5);
    CLEAR_BIT(P5M0, 5);
    CLEAR_BIT(P_SW2, 7);
    break;
  }

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
  CLEAR_BIT(TMOD, 7);         // GATE = 0
  CLEAR_BIT(TMOD, 6);         // C/T = 0 (timer mode)
  CLEAR_BIT(TMOD, 5);         // M1 = 0
  CLEAR_BIT(TMOD, 4);         // M0 = 0  -> Mode 0

  // Calculate reload value
  reload = calculate_timer1_reload(baud);

  // Set reload value (write when TR1=0 sets both TH1/TL1 and RL_TH1/RL_TL1)
  TH1 = (uint8_t)(reload >> 8);   // High byte
  TL1 = (uint8_t)(reload & 0xFF); // Low byte

  // Clear Timer1 overflow flag
  CLEAR_BIT(TCON, 7); // TF1 = 0

  // Start Timer1
  SET_BIT(TCON, 6); // TR1 = 1

  // Reset RX buffer
  rx_head = 0;
  rx_tail = 0;

  // Enable UART interrupt
  SET_BIT(IE, 4); // ES = 1
  SET_BIT(IE, 7); // EA = 1
}

static void serial_begin(uint32_t baud)
{
  // Use default pins
  serial_begin_with_pins(baud, UART_PINS_DEFAULT);
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

// Blocking write - waits for previous transmission to complete
static void serial_write(uint8_t byte)
{
  SBUF = byte; // Send the byte
  // Wait for previous transmission to complete
  while (!READ_BIT(SCON, 1)) // Wait while TI is 0
    ;

  CLEAR_BIT(SCON, 1); // Clear TI flag
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

// Print a number (supports int32_t)
static void serial_print_number(int32_t num) __reentrant
{
  char buffer[12]; // -2147483648 is 11 chars + null
  char *ptr = buffer + sizeof(buffer) - 1;
  uint32_t abs_num;
  uint8_t is_negative = 0;

  *ptr = '\0';

  // Handle negative numbers
  if (num < 0)
  {
    is_negative = 1;
    abs_num = (uint32_t)(-num);
  }
  else
  {
    abs_num = (uint32_t)num;
  }

  // Handle zero case
  if (abs_num == 0)
  {
    *(--ptr) = '0';
  }
  else
  {
    // Convert to string (reverse order)
    while (abs_num > 0)
    {
      *(--ptr) = '0' + (abs_num % 10);
      abs_num /= 10;
    }
  }

  // Add negative sign if needed
  if (is_negative)
  {
    *(--ptr) = '-';
  }

  // Print the string
  serial_print(ptr);
}

// Read string until newline or timeout (simple version)
static void serial_read_string(char *buffer, uint8_t max_len) __reentrant
{
  uint8_t index = 0;
  int c;
  uint16_t timeout = 0;
  const uint16_t max_timeout = 10000; // Adjust as needed

  while (index < (max_len - 1))
  {
    c = serial_read();

    if (c == -1)
    {
      // No data available, increment timeout
      timeout++;
      if (timeout >= max_timeout)
      {
        break;
      }
      continue;
    }

    // Reset timeout when data received
    timeout = 0;

    // Check for newline or carriage return
    if (c == '\n' || c == '\r')
    {
      break;
    }

    // Add character to buffer
    buffer[index++] = (char)c;
  }

  // Null terminate
  buffer[index] = '\0';
}

static char *serial_read_line(void) __reentrant
{
  uint8_t index = 0;
  int c;
  uint16_t timeout = 0;
  const uint16_t max_timeout = 10000;

  while (index < (sizeof(serial_line_buffer) - 1))
  {
    c = serial_read();

    if (c == -1)
    {
      timeout++;
      if (timeout >= max_timeout)
      {
        break;
      }
      continue;
    }

    timeout = 0;

    if (c == '\n' || c == '\r')
    {
      break;
    }

    serial_line_buffer[index++] = (char)c;
  }

  serial_line_buffer[index] = '\0';
  return serial_line_buffer;
}

// Serial object instance
Serial_t Serial = {
    .begin = serial_begin,
    .beginWithPins = serial_begin_with_pins,
    .end = serial_end,
    .available = serial_available,
    .read = serial_read,
    .write = serial_write,
    .print = serial_print,
    .println = serial_println,
    .printNumber = serial_print_number,
    .readString = serial_read_string,
    .readLine = serial_read_line,
};