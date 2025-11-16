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

## Usage

1. Select **Tools → Board → STC Boards → STC8G1K08A**
2. Select your clock frequency under **Tools → Clock**
3. Select your serial port under **Tools → Port**
4. Upload your sketch

## Arduino API Reference

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

### External Interrupts

> Note: Note: P3_2 and P3_3 can using FALLING, RISING or CHANGE modes, while other pins only support FALLING mode.

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
| P3_2 |2| P3.2 | GPIO, RX (UART alt), INT0 |
| P3_3 |3| P3.3 | GPIO, TX (UART alt), INT1 |
| P5_4 |4| P5.4 | GPIO, RX (UART alt), INT2 |
| P5_5 |5| P5.5 | GPIO, TX (UART alt), INT3 |

## Examples

The core includes several examples demonstrating basic functionality:

- **Blink** - Basic LED blinking
- **Button** - Reading digital input
- **Interrupt** - Using external interrupts
- **Millis_Micros** - Non-blocking timing with millis()/micros()
- **Serial** - Serial communication and echo

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
- Use `__reentrant` for recursive functions
- Global variables default to internal RAM (limited to 256 bytes)

<!-- ## Troubleshooting

### Upload Issues
- **No response from MCU:** Ensure power cycle during upload
- **Wrong baud rate:** Try different clock frequencies
- **Permission denied (Linux):** Add user to dialout group: `sudo usermod -a -G dialout $USER`

### Compilation Errors
- **Sketch too big:** Reduce code size or remove unused libraries
- **Out of memory:** Use `__xdata` for large arrays
- **Undefined reference:** Ensure all functions are declared before use -->

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## License

This project is open source. See individual file headers for specific licenses.

## Resources

- **GitHub Repository:** https://github.com/thevien257/STC_Arduino_Core
- **STC8G Documentation:** (https://www.stcmicro.com/stc/stc8g1k08.html
- **Arduino Reference:** https://www.arduino.cc/reference/en/

## Credits

Created by thevien257  
Based on SDCC compiler and stcgal programmer