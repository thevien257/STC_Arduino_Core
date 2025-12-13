#include "Arduino.h"

// Buffer for received data - moved to XRAM to save internal RAM
#define I2C_BUFFER_SIZE 32
static volatile __xdata uint8_t rx_buffer[I2C_BUFFER_SIZE];
static volatile uint8_t rx_buffer_head = 0;
static volatile uint8_t rx_buffer_tail = 0;

// Global state variables - moved to XRAM to save internal RAM
static __xdata I2cPinSelect_t current_pins = I2C_PINS_P32_P33;
static __xdata uint32_t current_clock = I2C_CLOCK_100KHZ;
static __xdata bool transmission_begun = false;
static __xdata uint8_t tx_address = 0;

// Helper functions
static void i2c_wait(void) {
    uint16_t timeout = 5000;
    while (!(I2CMSST & I2C_MSIF) && timeout--);
    I2CMSST &= ~I2C_MSIF;  // Clear interrupt flag
}

static void i2c_send_start(void) {
    I2CMSCR = I2C_CMD_START;
    i2c_wait();
}

static void i2c_send_stop(void) {
    I2CMSCR = I2C_CMD_STOP;
    i2c_wait();
}

static void i2c_send_byte(uint8_t data) {
    I2CTXD = data;
    I2CMSCR = I2C_CMD_SENDDATA;
    i2c_wait();
}

static uint8_t i2c_recv_byte(void) {
    I2CMSCR = I2C_CMD_RECVDATA;
    i2c_wait();
    return I2CRXD;
}

static void i2c_recv_ack(void) {
    I2CMSCR = I2C_CMD_RECVACK;
    i2c_wait();
}

static void i2c_send_ack(void) {
    I2CMSST = 0x00;  // ACK
    I2CMSCR = I2C_CMD_SENDACK;
    i2c_wait();
}

static void i2c_send_nak(void) {
    I2CMSST = 0x01;  // NAK
    I2CMSCR = I2C_CMD_SENDACK;
    i2c_wait();
}

// Calculate I2C speed setting
static uint8_t calculate_speed(uint32_t frequency) {
    // Formula: I2C_speed = F_CPU / 2 / (MSSPEED * 2 + 4)
    // Solve for MSSPEED: MSSPEED = (F_CPU / frequency / 2 - 4) / 2
    uint8_t msspeed = (F_CPU / frequency / 2 - 4) / 2;
    if (msspeed > 63) msspeed = 63;  // Max value is 6 bits
    return msspeed;
}

// Begin I2C with custom pins
static void i2c_beginWithPins(I2cPinSelect_t pins) {
    current_pins = pins;
    
    // Configure pins as open-drain with pull-up
    if (pins == I2C_PINS_P54_P55) {
        pinMode(P5_4, OUTPUT_OD_PU);  // SCL
        pinMode(P5_5, OUTPUT_OD_PU);  // SDA
        I2C_SWITCH_PINS(I2C_S_P54_P55);
    } else {
        pinMode(P3_2, OUTPUT_OD_PU);  // SCL
        pinMode(P3_3, OUTPUT_OD_PU);  // SDA
        I2C_SWITCH_PINS(I2C_S_P32_P33);
    }
    
    // Enable XFR for I2C register access
    ENABLE_XFR();
    
    // Configure I2C with default 100kHz
    uint8_t speed = calculate_speed(current_clock);
    I2CCFG = I2C_ENI2C | I2C_MASTER | speed;
    I2CMSST = 0x00;
    
    // Reset buffers
    rx_buffer_head = 0;
    rx_buffer_tail = 0;
    transmission_begun = false;
}

// Begin I2C with default settings (P3.2/P3.3, 100kHz)
static void i2c_begin(void) {
    i2c_beginWithPins(I2C_PINS_P32_P33);
}

// End I2C communication
static void i2c_end(void) {
    ENABLE_XFR();
    I2CCFG = 0x00;  // Disable I2C
}

// Set I2C clock frequency
static void i2c_setClock(uint32_t frequency) __reentrant {
    current_clock = frequency;
    
    ENABLE_XFR();
    uint8_t speed = calculate_speed(frequency);
    I2CCFG = I2C_ENI2C | I2C_MASTER | speed;
}

// Read one byte from buffer
static uint8_t i2c_read(void) {
    if (rx_buffer_head == rx_buffer_tail) {
        return 0;  // No data available
    }
    
    uint8_t data = rx_buffer[rx_buffer_tail];
    rx_buffer_tail = (rx_buffer_tail + 1) % I2C_BUFFER_SIZE;
    return data;
}

// Write one byte (queued until endTransmission)
static void i2c_write(uint8_t data) {
    if (!transmission_begun) {
        return;  // Must call beginTransmission first
    }
    
    i2c_send_byte(data);
    i2c_recv_ack();
}

// Begin transmission to slave device
static void i2c_beginTransmission(uint8_t address) {
    tx_address = address << 1;  // Convert 7-bit to 8-bit write address
    transmission_begun = true;
    
    i2c_send_start();
    i2c_send_byte(tx_address);
    i2c_recv_ack();
}

// End transmission
// sendStop: true = send STOP condition, false = repeated START
// Returns: 0 = success, 1 = data too long, 2 = NACK on address, 3 = NACK on data, 4 = other error
static uint8_t i2c_endTransmission(bool sendStop) {
    if (!transmission_begun) {
        return 4;  // No transmission in progress
    }
    
    if (sendStop) {
        i2c_send_stop();
    }
    
    transmission_begun = false;
    return 0;  // Success
}

// Request bytes from slave device
// Returns: number of bytes read
static uint8_t i2c_requestFrom(uint8_t address, uint8_t quantity, bool sendStop) __reentrant {
    if (quantity > I2C_BUFFER_SIZE) {
        quantity = I2C_BUFFER_SIZE;
    }
    
    // Send START + read address
    i2c_send_start();
    i2c_send_byte((address << 1) | 0x01);  // Read address
    i2c_recv_ack();
    
    // Read bytes
    uint8_t count = 0;
    for (uint8_t i = 0; i < quantity; i++) {
        uint8_t data = i2c_recv_byte();
        
        // Store in buffer
        uint8_t next_head = (rx_buffer_head + 1) % I2C_BUFFER_SIZE;
        if (next_head != rx_buffer_tail) {
            rx_buffer[rx_buffer_head] = data;
            rx_buffer_head = next_head;
            count++;
        }
        
        // Send ACK for all except last byte
        if (i < quantity - 1) {
            i2c_send_ack();
        } else {
            i2c_send_nak();
        }
    }
    
    if (sendStop) {
        i2c_send_stop();
    }
    
    return count;
}

// Get number of bytes available in receive buffer
static uint8_t i2c_available(void) {
    return (I2C_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail) % I2C_BUFFER_SIZE;
}

// I2C object instance
I2C_t Wire = {
    .begin = i2c_begin,
    .beginWithPins = i2c_beginWithPins,
    .end = i2c_end,
    .setClock = i2c_setClock,
    .read = i2c_read,
    .write = i2c_write,
    .beginTransmission = i2c_beginTransmission,
    .endTransmission = i2c_endTransmission,
    .requestFrom = i2c_requestFrom,
    .available = i2c_available,
};