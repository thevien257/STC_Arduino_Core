/*
  STC8G1K08A I2C RTC Communication
  Reads time from DS1307/DS3231 RTC module
  
  Hardware connections:
  - P5.5 (SDA)
  - P5.4 (SCL)
*/

// RTC I2C addresses
#define RTC_ADDR_W 0xD0
#define RTC_ADDR_R 0xD1

// RTC register addresses
#define RTC_SEC_REG 0x00
#define RTC_MIN_REG 0x01
#define RTC_HOUR_REG 0x02
#define RTC_DAY_REG 0x03
#define RTC_DATE_REG 0x04
#define RTC_MONTH_REG 0x05
#define RTC_YEAR_REG 0x06

// Time structure
typedef struct {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint8_t day;
  uint8_t date;
  uint8_t month;
  uint8_t year;
} RTC_Time;

RTC_Time current_time;

// I2C Functions
void i2c_wait(void) {
  uint16_t timeout = 5000;
  while (!(I2CMSST & 0x40) && timeout--)
    ;
  I2CMSST &= ~0x40;
}

void i2c_start(void) {
  I2CMSCR = 0x01;
  i2c_wait();
}

void i2c_stop(void) {
  I2CMSCR = 0x06;
  i2c_wait();
}

void i2c_send_byte(uint8_t dat) {
  I2CTXD = dat;
  I2CMSCR = 0x02;
  i2c_wait();
}

uint8_t i2c_recv_byte(void) {
  I2CMSCR = 0x04;
  i2c_wait();
  return I2CRXD;
}

void i2c_send_ack(void) {
  I2CMSST = 0x00;
  I2CMSCR = 0x05;
  i2c_wait();
}

void i2c_send_nak(void) {
  I2CMSST = 0x01;
  I2CMSCR = 0x05;
  i2c_wait();
}

void i2c_recv_ack(void) {
  I2CMSCR = 0x03;
  i2c_wait();
}

// Initialize I2C on P5.4 (SCL) and P5.5 (SDA)
void i2c_init(void) {
  // Configure I2C pins with open-drain + pull-up
  pinMode(P5_4, OUTPUT_OD_PU);  // SCL
  pinMode(P5_5, OUTPUT_OD_PU);  // SDA

  // Switch I2C to P5.4/P5.5
  I2C_SWITCH_PINS(I2C_S_P54_P55);

  // Enable XFR for I2C register access
  ENABLE_XFR();

  // Configure I2C: Enable + Master mode + speed
  I2CCFG = 0xC0 | 25;  // 100kHz @ 11.0592MHz
  I2CMSST = 0x00;

  // Keep XFR enabled for I2C operations
}

// BCD conversion
uint8_t bcd_to_dec(uint8_t bcd) {
  return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t dec_to_bcd(uint8_t dec) {
  return ((dec / 10) << 4) | (dec % 10);
}

// Write to RTC register
void rtc_write(uint8_t reg, uint8_t data) {
  i2c_start();
  i2c_send_byte(RTC_ADDR_W);
  i2c_recv_ack();
  i2c_send_byte(reg);
  i2c_recv_ack();
  i2c_send_byte(data);
  i2c_recv_ack();
  i2c_stop();
}

// Read from RTC register
uint8_t rtc_read(uint8_t reg) {
  uint8_t data;

  i2c_start();
  i2c_send_byte(RTC_ADDR_W);
  i2c_recv_ack();
  i2c_send_byte(reg);
  i2c_recv_ack();

  i2c_start();
  i2c_send_byte(RTC_ADDR_R);
  i2c_recv_ack();
  data = i2c_recv_byte();
  i2c_send_nak();
  i2c_stop();

  return data;
}

// Set RTC time
void rtc_set_time(RTC_Time *time) {
  rtc_write(RTC_SEC_REG, dec_to_bcd(time->seconds));
  rtc_write(RTC_MIN_REG, dec_to_bcd(time->minutes));
  rtc_write(RTC_HOUR_REG, dec_to_bcd(time->hours));
  rtc_write(RTC_DAY_REG, dec_to_bcd(time->day));
  rtc_write(RTC_DATE_REG, dec_to_bcd(time->date));
  rtc_write(RTC_MONTH_REG, dec_to_bcd(time->month));
  rtc_write(RTC_YEAR_REG, dec_to_bcd(time->year));
}

// Get RTC time
void rtc_get_time(RTC_Time *time) {
  time->seconds = bcd_to_dec(rtc_read(RTC_SEC_REG) & 0x7F);
  time->minutes = bcd_to_dec(rtc_read(RTC_MIN_REG) & 0x7F);
  time->hours = bcd_to_dec(rtc_read(RTC_HOUR_REG) & 0x3F);
  time->day = bcd_to_dec(rtc_read(RTC_DAY_REG) & 0x07);
  time->date = bcd_to_dec(rtc_read(RTC_DATE_REG) & 0x3F);
  time->month = bcd_to_dec(rtc_read(RTC_MONTH_REG) & 0x1F);
  time->year = bcd_to_dec(rtc_read(RTC_YEAR_REG));
}

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  Serial.println("STC8G I2C RTC Test");

  // Initialize I2C
  i2c_init();

  // Set initial time: 12:30:00, Saturday (7), 13/Dec/2025
  current_time.seconds = 0;
  current_time.minutes = 30;
  current_time.hours = 12;
  current_time.day = 7;  // Saturday
  current_time.date = 13;
  current_time.month = 12;
  current_time.year = 25;  // 2024

  rtc_set_time(&current_time);

  Serial.println("RTC initialized!");
  delay_ms(1000);
}

void loop() {
  // Read time from RTC
  rtc_get_time(&current_time);

  // Print time to serial
  Serial.print("Time: ");
  Serial.printNumber(current_time.hours);
  Serial.print(":");
  if (current_time.minutes < 10) Serial.print("0");
  Serial.printNumber(current_time.minutes);
  Serial.print(":");
  if (current_time.seconds < 10) Serial.print("0");
  Serial.printNumber(current_time.seconds);

  Serial.print("  Date: ");
  Serial.printNumber(current_time.date);
  Serial.print("/");
  Serial.printNumber(current_time.month);
  Serial.print("/20");
  Serial.printNumber(current_time.year);

  Serial.println("");

  // Display on ports (for logic analyzer/LED monitoring)
  P0 = current_time.seconds;
  P1 = current_time.minutes;
  P2 = current_time.hours;

  delay_ms(1000);
}