// SSD1306 OLED Display Example using MicroI2CMaster

#include <Arduino.h>
#include "MicroI2CMaster.h"

#define DEBUG

#if defined(DEBUG) && !defined(ARDUINO_attiny)
#define SERIAL_BEGIN(x) Serial.begin(x)
#if defined(ARDUINO_AVR_ATTINYX5)
#define SERIAL_AVAILABLE()
#else
#define SERIAL_AVAILABLE() while (!Serial)
#endif
#define SERIAL_PRINT(x) Serial.print(x)
#define SERIAL_PRINTLN(x) Serial.println(x)
#define SERIAL_PRINTHEX(x) Serial.print(x, HEX)
#define SERIAL_PRINTLNHEX(x) Serial.println(x, HEX)
#else
#define SERIAL_BEGIN(x)
#define SERIAL_AVAILABLE()
#define SERIAL_PRINT(x)
#define SERIAL_PRINTLN(x)
#define SERIAL_PRINTHEX(x)
#define SERIAL_PRINTLNHEX(x)
#endif

#define SSD1306_ADDRESS 0x3C
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D

typedef char (*char_getter_t)(const char* str, uint8_t index);

const uint8_t font5x8[] PROGMEM = {
  // ! (0x21)
  0x00, 0x00, 0x5F, 0x00, 0x00,
  // E (0x45)
  0x7F, 0x49, 0x49, 0x49, 0x41,
  // H (0x48)
  0x7F, 0x08, 0x08, 0x08, 0x7F,
  // L (0x4C)
  0x7F, 0x40, 0x40, 0x40, 0x40,
  // O (0x4F)
  0x3E, 0x41, 0x41, 0x41, 0x3E,
};

void setup() {
  SERIAL_BEGIN(9600);
  SERIAL_AVAILABLE();
  SERIAL_PRINTLN("--");

  MicroI2CMaster_begin();
  delay(100);

  ssd1306_init();
  delay(100);

  SERIAL_PRINTLN("SSD1306");
}

void loop() {
  ssd1306_clear();
  ssd1306_drawString(32, 3, "HELLO!", getChar);
  delay(5 * 1000);
}

static inline void ssd1306_command(uint8_t cmd) {
  MicroI2CMaster_start();
  MicroI2CMaster_write(SSD1306_ADDRESS << 1);
  MicroI2CMaster_write(0x00);
  MicroI2CMaster_write(cmd);
  MicroI2CMaster_stop();
}

static inline void ssd1306_data(uint8_t data) {
  MicroI2CMaster_start();
  MicroI2CMaster_write(SSD1306_ADDRESS << 1);
  MicroI2CMaster_write(0x40);
  MicroI2CMaster_write(data);
  MicroI2CMaster_stop();
}

static inline void ssd1306_init() {
  ssd1306_command(SSD1306_DISPLAYOFF);
  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);
  ssd1306_command(0x80);
  ssd1306_command(SSD1306_SETMULTIPLEX);
  ssd1306_command(0x3F);
  ssd1306_command(SSD1306_SETDISPLAYOFFSET);
  ssd1306_command(0x00);
  ssd1306_command(SSD1306_SETSTARTLINE | 0x00);
  ssd1306_command(SSD1306_CHARGEPUMP);
  ssd1306_command(0x14);
  ssd1306_command(SSD1306_MEMORYMODE);
  ssd1306_command(0x00);
  ssd1306_command(SSD1306_SEGREMAP | 0x01);
  ssd1306_command(SSD1306_COMSCANDEC);
  ssd1306_command(SSD1306_SETCOMPINS);
  ssd1306_command(0x12);
  ssd1306_command(SSD1306_SETCONTRAST);
  ssd1306_command(0xCF);
  ssd1306_command(SSD1306_SETPRECHARGE);
  ssd1306_command(0xF1);
  ssd1306_command(SSD1306_SETVCOMDETECT);
  ssd1306_command(0x40);
  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);
  ssd1306_command(SSD1306_NORMALDISPLAY);
  ssd1306_command(SSD1306_DISPLAYON);
}

static inline void ssd1306_clear() {
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);
  ssd1306_command(127);
  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(0);
  ssd1306_command(7);

  for (uint16_t i = 0; i < 1024; i++) { ssd1306_data(0x00); }
}

static inline void ssd1306_drawChar(uint8_t x, uint8_t page, char c) {
  uint8_t charIndex = getCharIndex(c);

  for (uint8_t row = 0; row < 2; row++) {
    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(page + row);
    ssd1306_command(page + row);

    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(x);
    ssd1306_command(x + 9);

    for (uint8_t i = 0; i < 5; i++) {
      uint8_t fontByte = pgm_read_byte(&font5x8[charIndex * 5 + i]);
      uint8_t expandedByte = 0;

      uint8_t srcBits = (row == 0) ? (fontByte & 0x0F) : (fontByte >> 4);

      if (srcBits & 0x01) expandedByte |= 0x01;
      if (srcBits & 0x02) expandedByte |= 0x04;
      if (srcBits & 0x04) expandedByte |= 0x10;
      if (srcBits & 0x08) expandedByte |= 0x40;

      ssd1306_data(expandedByte);
      ssd1306_data(expandedByte);
    }
  }
}

static inline void ssd1306_drawString(uint8_t x, uint8_t page, const char* str, char_getter_t getter) {
  uint8_t pos = x;
  uint8_t index = 0;
  char c;
  while ((c = getter(str, index++)) != '\0') {
    ssd1306_drawChar(pos, page, c);
    pos += 11;
  }
}

static inline char getChar(const char* str, uint8_t index) {
  return pgm_read_byte(str + index);
}

static inline uint8_t getCharIndex(char c) {
  if (c == '!') return 0;
  if (c == 'E') return 1;
  if (c == 'H') return 2;
  if (c == 'L') return 3;
  if (c == 'O') return 4;
  return 0;
}
