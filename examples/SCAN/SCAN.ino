// SCAN - I2C Device Scanner Example

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

uint8_t data[16];

void setup() {
  SERIAL_BEGIN(9600);
  SERIAL_AVAILABLE();
  SERIAL_PRINTLN("--");

  MicroI2CMaster_begin();
  delay(100);

  SERIAL_PRINTLN("SCAN");
}

void loop() {
  SERIAL_PRINTLN("--");

  uint8_t count = MicroI2CMaster_scan(data, sizeof(data));
  SERIAL_PRINT("Number of devices: ");
  SERIAL_PRINTLN(count);

  if (count) {
    for (uint8_t i = 0; i < count; i++) {
      SERIAL_PRINT("0x");
      if (data[i] < 16) {
        SERIAL_PRINT("0");
      }
      SERIAL_PRINTHEX(data[i]);
      SERIAL_PRINT(" (");
      SERIAL_PRINT(data[i]);
      SERIAL_PRINTLN(")");
    }
  }

  SERIAL_PRINTLN("");
  delay(3 * 1000);
}
