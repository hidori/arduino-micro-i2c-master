// AHTX0 - Temperature and Humidity Sensor Example

#include <Arduino.h>
#include "MicroI2CMaster.h"

#define DEBUG

#define AHTX0_I2C_ADDRESS 0x38
#define AHTX0_CMD_SOFTRESET 0xBA
#define AHTX0_CMD_CALIBRATE 0xBE
#define AHTX0_CMD_TRIGGER 0xAC

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

uint8_t data[8];

void setup() {
  SERIAL_BEGIN(9600);
  SERIAL_AVAILABLE();
  SERIAL_PRINTLN("--");

  MicroI2CMaster_begin();
  delay(100);

  ahtX0_write_cmd(AHTX0_CMD_SOFTRESET, 0x00, 0x00);
  delay(20);
  ahtX0_write_cmd(AHTX0_CMD_CALIBRATE, 0x08, 0x00);
  delay(10);

  SERIAL_PRINTLN("AHTX0");
}

void loop() {
  ahtX0_write_cmd(AHTX0_CMD_TRIGGER, 0x33, 0x00);
  delay(80);

  if (ahtX0_read_data(data)) {
    float temperature, humidity;
    ahtX0_calc_temp_hum(data, &temperature, &humidity);
    SERIAL_PRINT(temperature);
    SERIAL_PRINT("C ");
    SERIAL_PRINT(humidity);
    SERIAL_PRINTLN("%");
  } else {
    SERIAL_PRINTLN("Fail to read data");
  }

  delay(3 * 1000);
}

static inline void ahtX0_write_cmd(uint8_t cmd, uint8_t data1, uint8_t data2) {
  MicroI2CMaster_start();
  MicroI2CMaster_write((AHTX0_I2C_ADDRESS << 1) | 0);
  MicroI2CMaster_write(cmd);
  MicroI2CMaster_write(data1);
  MicroI2CMaster_write(data2);
  MicroI2CMaster_stop();
}

static inline bool ahtX0_read_data(uint8_t data[6]) {
  MicroI2CMaster_start();
  MicroI2CMaster_write((AHTX0_I2C_ADDRESS << 1) | 1);
  for (uint8_t i = 0; i < 6; i++) { MicroI2CMaster_read(&data[i], (i < 5) ? true : false); }
  MicroI2CMaster_stop();
  return ((data[0] & 0x80) == 0) && !(data[0] == 0xFF && data[1] == 0xFF && data[2] == 0xFF);
}

static inline void ahtX0_calc_temp_hum(uint8_t* data, float* temperature, float* humidity) {
  uint32_t raw_temp = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
  *temperature = ((float)raw_temp / 1048576.0) * 200.0 - 50.0;

  uint32_t raw_hum = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
  *humidity = ((float)raw_hum * 100) / 1048576.0;
}
