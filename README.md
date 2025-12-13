# STC8 Arduino Core

Arduino support for STC8 microcontrollers with a familiar Arduino-like API.

## Installation

### Via Arduino Board Manager (Recommended)

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add this URL to **Additional Boards Manager URLs**:
   ```
   https://raw.githubusercontent.com/thevien257/STC_Arduino_Core/main/package_stc8051_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search for "STC"
6. Click **Install** on "STC Boards"

### Requirements

- Python 3.6 or later (for stcgal programmer)
- Arduino IDE 1.8.x or Arduino IDE 2.x

### Supported Boards

- STC8G1K08A (8KB Flash, 1KB RAM)

### OS Supported
- Windows
- Linux

## Usage

1. Select **Tools → Board → STC Boards → STC8G1K08A-8PIN**
2. Select your clock frequency under **Tools → Clock Speed**
3. Select your serial port under **Tools → Port**
4. Upload your sketch

## Arduino API Reference
Below are the APIs that have been successfully tested. More APIs are currently under development.

### Digital I/O

```cpp
// Pin definitions
P3_0, P3_1, P3_2, P3_3  // Port 3 pins
P5_4, P5_5              // Port 5 pins

// Pin modes
pinMode(pin, mode);
// Modes: OUTPUT, INPUT, INPUT_PULLUP

// Digital read/write
digitalWrite(pin, value);  // HIGH or LOW
int value = digitalRead(pin);
digitalToggle(pin);        // Toggle pin state
```

**Example:**
```cpp
void setup() {
  pinMode(P5_5, OUTPUT);
  pinMode(P3_2, INPUT_PULLUP);
}

void loop() {
  digitalWrite(P5_5, HIGH);
  delay(1000);
  digitalWrite(P5_5, LOW);
  delay(1000);
}
```

### Timing Functions

```cpp
// Blocking delays
delay_ms(milliseconds);    // Delay in milliseconds
delay_us(microseconds);    // Delay in microseconds
delay_s(seconds);          // Delay in seconds

// Non-blocking timing
uint32_t ms = millis();    // Milliseconds since startup
uint32_t us = micros();    // Microseconds since startup
```

**Example:**
```cpp
void loop() {
  static uint32_t previousMillis = 0;
  uint32_t currentMillis = millis();
  
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    digitalToggle(P5_5);
  }
}
```

### Serial Communication

```cpp
// Initialize serial
Serial.begin(115200);

// Alternative UART pins
Serial.beginWithPins(115200, UART_PINS_DEFAULT);  // TX=P3.1, RX=P3.0
Serial.beginWithPins(115200, UART_PINS_P32_P33);  // TX=P3.3, RX=P3.2
Serial.beginWithPins(115200, UART_PINS_P54_P55);  // TX=P5.5, RX=P5.4

// Print functions
Serial.print("Hello");
Serial.println("World");
Serial.printNumber(42);
Serial.printNumber(-123);

// Read functions
int available = Serial.available();  // Bytes available
int data = Serial.read();            // Read single byte (-1 if none)
char* line = Serial.readLine();      // Read line (blocking, with timeout)
Serial.readString(buffer, max_len);  // Read into buffer

// Write
Serial.write(0x42);  // Write single byte

// Close
Serial.end();
```

**Example:**
```cpp
void setup() {
  Serial.begin(115200);
  Serial.println("STC8 Serial Test");
}

void loop() {
  if (Serial.available() > 0) {
    char* line = Serial.readLine();
    Serial.print("Echo: ");
    Serial.println(line);
  }
}
```

### I2C Communication

The Wire library provides I2C master mode communication compatible with Arduino's Wire API.

```cpp
// Include I2C library
#include "i2c.h"

// Initialize I2C
Wire.begin();                              // Default pins (P3.2=SCL, P3.3=SDA), 100kHz
Wire.beginWithPins(I2C_PINS_P54_P55);     // Alternative pins (P5.4=SCL, P5.5=SDA)

// Configure I2C clock
Wire.setClock(I2C_CLOCK_100KHZ);          // 100 kHz (standard mode)
Wire.setClock(I2C_CLOCK_400KHZ);          // 400 kHz (fast mode)

// Write to I2C device
Wire.beginTransmission(address);           // Start transmission (7-bit address)
Wire.write(data);                          // Write byte
Wire.endTransmission(true);                // End with STOP (true) or repeated START (false)

// Read from I2C device
Wire.requestFrom(address, quantity, true); // Request bytes, end with STOP
while (Wire.available()) {
  uint8_t data = Wire.read();              // Read received bytes
}

// Close I2C
Wire.end();
```

**Pin Selection:**
- `I2C_PINS_P32_P33` - SCL=P3.2, SDA=P3.3 (default)
- `I2C_PINS_P54_P55` - SCL=P5.4, SDA=P5.5

### RTC (DS1307/DS3231) Library

A simple library for interfacing with DS1307 and DS3231 Real-Time Clock modules via I2C.

```cpp
// Include RTC library
#include "rtc_ds1307.h"

// Initialize RTC
RTC_begin(I2C_PINS_P32_P33);  // or I2C_PINS_P54_P55

// Set time
RTC_setTime(year, month, date, hours, minutes, seconds, day);
// year: 0-99 (2000-2099)
// month: 1-12
// date: 1-31
// hours: 0-23 (24-hour format)
// minutes: 0-59
// seconds: 0-59
// day: RTC_SUNDAY to RTC_SATURDAY (1-7)

// Get time
uint8_t year, month, date, hours, minutes, seconds, day;
RTC_getTime(&year, &month, &date, &hours, &minutes, &seconds, &day);

// Control oscillator
bool running = RTC_isRunning();  // Check if clock is running
RTC_start();                      // Start the clock
RTC_stop();                       // Stop the clock (saves battery)
```

**Day of Week Constants:**
```cpp
RTC_SUNDAY    // 1
RTC_MONDAY    // 2
RTC_TUESDAY   // 3
RTC_WEDNESDAY // 4
RTC_THURSDAY  // 5
RTC_FRIDAY    // 6
RTC_SATURDAY  // 7
```

**Complete RTC Example:**
```cpp
#include "rtc_ds1307.h"

void setup() {
  Serial.begin(115200);
  Serial.println("RTC Test");
  
  // Initialize RTC on P5.4/P5.5
  if (RTC_begin(I2C_PINS_P54_P55)) {
    Serial.println("RTC initialized");
    
    // Set time: 12:30:00, Saturday, Dec 13, 2025
    RTC_setTime(25, 12, 13, 12, 30, 0, RTC_SATURDAY);
  } else {
    Serial.println("RTC not found!");
  }
}

void loop() {
  uint8_t year, month, date, hours, minutes, seconds, day;
  
  if (RTC_getTime(&year, &month, &date, &hours, &minutes, &seconds, &day)) {
    // Print time
    Serial.print("Time: ");
    Serial.printNumber(hours);
    Serial.print(":");
    if (minutes < 10) Serial.print("0");
    Serial.printNumber(minutes);
    Serial.print(":");
    if (seconds < 10) Serial.print("0");
    Serial.printNumber(seconds);
    
    // Print date
    Serial.print("  Date: ");
    Serial.printNumber(date);
    Serial.print("/");
    Serial.printNumber(month);
    Serial.print("/20");
    Serial.printNumber(year);
    Serial.println("");
  }
  
  delay_ms(1000);
}
```

### External Interrupts

> Note: P3_2 and P3_3 can using FALLING, RISING or CHANGE modes, while other pins only support FALLING mode.

```cpp
// Interrupt modes
FALLING  // Trigger on falling edge
CHANGE   // Trigger on both edges (INT0/INT1 only)
RISING   // Same as CHANGE on STC8

// Attach interrupt
attachInterrupt(interrupt, isr_function, mode);
detachInterrupt(interrupt);

// Get interrupt number from pin
int intNum = digitalPinToInterrupt(pin);

// Supported pins
P3_2  // INT0
P3_3  // INT1
P5_4  // INT2
P5_5  // INT3
P3_0  // INT4
```

**Example:**
```cpp
volatile uint8_t flag = 0;

void button_isr() {
  flag = 1;
}

void setup() {
  pinMode(P3_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(P3_2), button_isr, FALLING);
}

void loop() {
  if (flag) {
    flag = 0;
    Serial.println("Button pressed!");
  }
}
```

## Pin Mapping

| Arduino Pin |Pin Number| Physical Pin | Functions |
|------------|-----|--------------|-----------|
| P3_0 |0| P3.0 | GPIO, RX (UART alt), INT4 |
| P3_1 |1| P3.1 | GPIO, TX (UART alt) |
| P3_2 |2| P3.2 | GPIO, RX (UART alt), INT0, I2C SCL |
| P3_3 |3| P3.3 | GPIO, TX (UART alt), INT1, I2C SDA |
| P5_4 |4| P5.4 | GPIO, RX (UART alt), INT2, I2C SCL |
| P5_5 |5| P5.5 | GPIO, TX (UART alt), INT3, I2C SDA |

## Examples

The core includes several examples demonstrating basic functionality:

- **Blink** - Basic LED blinking
- **Button** - Reading digital input
- **Interrupt** - Using external interrupts
- **Millis_Micros** - Non-blocking timing with millis()/micros()
- **Serial** - Serial communication and echo
- **I2C_Scanner** - Scan for I2C devices on the bus
- **RTC_Clock** - Real-time clock with DS1307/DS3231

Access examples via: **File → Examples → 1. STC8G Examples**

## Uploading Sketches

### Connection

Connect a USB-to-TTL adapter to your STC8 board:

| Adapter | STC8 Board |
|---------|------------|
| 3.3V | VCC |
| GND | GND |
| TX | RX (depends on UART pin selection) |
| RX | TX (depends on UART pin selection) |

### Upload Process

1. Click **Upload** in Arduino IDE
2. When prompted, **power cycle the board** (disconnect and reconnect GND)
3. Upload will proceed automatically

## Clock Frequencies

Supported internal RC oscillator frequencies:
- 11.0592 MHz (default)
- 12 MHz
- 16 MHz
- 20 MHz
- 22.1184 MHz
- 24 MHz
- 30 MHz
- 33.1776 MHz

Select frequency via **Tools → Clock Speed** menu.

## Programming Notes

### Memory Limitations
- **Flash:** 8 KB total
- **RAM:** 1 KB (256 bytes internal + 768 bytes XRAM)
- Use `__xdata` keyword for large buffers to save internal RAM

### SDCC Specific
- This core uses SDCC compiler
- Some C++ features may be limited
- Use `__reentrant` on functions with parameters that may be called from interrupts to ensure thread-safe execution under SDCC
- Global variables default to internal RAM (limited to 256 bytes)

### I2C Notes
- Maximum I2C buffer size: 32 bytes
- Supports 100kHz and 400kHz clock speeds
- Master mode only (slave mode not implemented)
- Always use open-drain with pull-up configuration for I2C pins

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## Resources

- **GitHub Repository:** https://github.com/thevien257/STC_Arduino_Core
- **STC8G Documentation:** https://www.stcmicro.com/stc/stc8g1k08.html
- **Arduino Reference:** https://www.arduino.cc/reference/en/

## Submit Bugs
I would appreciate if you could submit any bugs or issues you find on the [GitHub Issues Page](https://github.com/thevien257/STC_Arduino_Core/issues).