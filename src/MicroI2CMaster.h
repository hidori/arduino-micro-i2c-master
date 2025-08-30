// MicroI2CMaster.h - Lightweight bit-bang I2C Master library for AVR

/*
 * MIT License
 *
 * Copyright (c) 2025 Hiroaki SHIBUKI
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __MICRO_I2C_MASTER_H__
#define __MICRO_I2C_MASTER_H__

#include <stdbool.h>
#include <stddef.h>
#include <util/delay.h>
#include <avr/io.h>

// Timing definitions
#if !defined(MICRO_I2C_MASTER_DELAY)
#define MICRO_I2C_MASTER_DELAY() _delay_us(5)
#endif

// Default I2C SDA pin
#if !defined(MICRO_I2C_MASTER_SDA)
#define MICRO_I2C_MASTER_SDA PB2
#endif

// Default I2C SCL pin
#if !defined(MICRO_I2C_MASTER_SCL)
#define MICRO_I2C_MASTER_SCL PB3
#endif

// Set I2C SDA pin high
static inline void MicroI2CMaster_sda_high(void) {
  DDRB &= ~(1 << MICRO_I2C_MASTER_SDA);
  PORTB |= (1 << MICRO_I2C_MASTER_SDA);
}

// Set I2C SDA pin low
static inline void MicroI2CMaster_sda_low(void) {
  PORTB &= ~(1 << MICRO_I2C_MASTER_SDA);
  DDRB |= (1 << MICRO_I2C_MASTER_SDA);
}

// Set I2C SCL pin high
static inline void MicroI2CMaster_scl_high(void) {
  DDRB &= ~(1 << MICRO_I2C_MASTER_SCL);
  PORTB |= (1 << MICRO_I2C_MASTER_SCL);
}

// Set I2C SCL pin low
static inline void MicroI2CMaster_scl_low(void) {
  PORTB &= ~(1 << MICRO_I2C_MASTER_SCL);
  DDRB |= (1 << MICRO_I2C_MASTER_SCL);
}

// Set I2C SCL pin high and wait for clock stretching
static inline void MicroI2CMaster_scl_high_wait(void) {
  DDRB &= ~(1 << MICRO_I2C_MASTER_SCL);
  PORTB |= (1 << MICRO_I2C_MASTER_SCL);
  while (!(PINB & (1 << MICRO_I2C_MASTER_SCL)));
}

// Initialize I2C pins
static inline void MicroI2CMaster_begin(void) {
  PORTB &= ~((1 << MICRO_I2C_MASTER_SDA) | (1 << MICRO_I2C_MASTER_SCL));
  DDRB &= ~((1 << MICRO_I2C_MASTER_SDA) | (1 << MICRO_I2C_MASTER_SCL));
  MicroI2CMaster_sda_high();
  MicroI2CMaster_scl_high_wait();
}

// Generate I2C start condition
static inline void MicroI2CMaster_start(void) {
  MicroI2CMaster_sda_high();
  MICRO_I2C_MASTER_DELAY();
  MicroI2CMaster_scl_high_wait();
  MICRO_I2C_MASTER_DELAY();
  MicroI2CMaster_sda_low();
  MICRO_I2C_MASTER_DELAY();
  MicroI2CMaster_scl_low();
  MICRO_I2C_MASTER_DELAY();
}

// Generate I2C stop condition
static inline void MicroI2CMaster_stop(void) {
  MicroI2CMaster_scl_low();
  MICRO_I2C_MASTER_DELAY();
  MicroI2CMaster_sda_low();
  MICRO_I2C_MASTER_DELAY();
  MicroI2CMaster_scl_high_wait();
  MICRO_I2C_MASTER_DELAY();
  MicroI2CMaster_sda_high();
  MICRO_I2C_MASTER_DELAY();
}

// Read data byte, set ack to true to send ACK
static inline void MicroI2CMaster_read(uint8_t* data, bool ack) {
  *data = 0;
  MicroI2CMaster_sda_high();
  for (uint8_t i = 0; i < 8; i++) {
    MicroI2CMaster_scl_low();
    MICRO_I2C_MASTER_DELAY();
    MicroI2CMaster_scl_high_wait();
    MICRO_I2C_MASTER_DELAY();
    *data <<= 1;
    if (PINB & (1 << MICRO_I2C_MASTER_SDA)) { *data |= 1; }
  }
  MicroI2CMaster_scl_low();
  if (ack) {
    MicroI2CMaster_sda_low();
  } else {
    MicroI2CMaster_sda_high();
  }
  MICRO_I2C_MASTER_DELAY();
  MicroI2CMaster_scl_high_wait();
  MICRO_I2C_MASTER_DELAY();
  MicroI2CMaster_scl_low();
  MICRO_I2C_MASTER_DELAY();
  MicroI2CMaster_sda_high();
}

// Write data byte, returns true if ACK received
static inline bool MicroI2CMaster_write(uint8_t data) {
  for (uint8_t i = 0; i < 8; i++) {
    MicroI2CMaster_scl_low();
    if (data & 0x80) {
      MicroI2CMaster_sda_high();
    } else {
      MicroI2CMaster_sda_low();
    }
    MICRO_I2C_MASTER_DELAY();
    MicroI2CMaster_scl_high_wait();
    MICRO_I2C_MASTER_DELAY();
    data <<= 1;
  }
  MicroI2CMaster_scl_low();
  MicroI2CMaster_sda_high();
  MICRO_I2C_MASTER_DELAY();
  MicroI2CMaster_scl_high_wait();
  MICRO_I2C_MASTER_DELAY();
  bool ack = !(PINB & (1 << MICRO_I2C_MASTER_SDA));
  MicroI2CMaster_scl_low();
  MICRO_I2C_MASTER_DELAY();
  return ack;
}

// Scan I2C bus and return count of found devices
static inline uint8_t MicroI2CMaster_scan(uint8_t* data, uint8_t count) {
  size_t found_count = 0;
  for (uint8_t addr = 8; addr < 120; addr++) {
    MicroI2CMaster_start();
    bool result = MicroI2CMaster_write((addr << 1) | 0);
    MicroI2CMaster_stop();

    _delay_us(100);

    if (result) {
      if (data && found_count < count) {
        data[found_count] = addr;
      }
      found_count++;
      if (found_count >= count) {
        break;
      }
    }
  }
  return found_count;
}

#endif  // __MICRO_I2C_MASTER_H__
