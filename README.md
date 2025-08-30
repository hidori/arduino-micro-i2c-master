# Arduino: MicroI2CMaster - Lightweight bit-bang I2C Master library for AVR

A minimal I2C master implementation using bit-banging technique, optimized for ATtiny and ATmega microcontrollers.

## Features

- **Lightweight**: Header-only library with static inline functions
- **Bit-bang I2C**: No hardware I2C peripheral required
- **AVR-optimized**: Designed specifically for ATtiny and ATmega microcontrollers
- **Minimal memory footprint**: Optimized for resource-constrained microcontrollers
- **Simple API**: Easy to use functions for I2C communication

## Supported Microcontrollers (AVR Only)

- **ATtiny13** (MicroCore)
- **ATtiny85** (ATTinyCore, DigiSpark)
- **ATmega328P** (Arduino UNO/Nano)
- **ATmega32U4** (Arduino Pro Micro, Leonardo)
- Other AVR microcontrollers with compatible GPIO

**Note**: This library is specifically designed for AVR architecture and uses AVR-specific register operations (DDRB, PORTB, PINB). It will not work on ESP32, RP2040, or other non-AVR platforms.

## Installation

### Arduino IDE Library Manager

1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for "MicroI2CMaster"
4. Click Install

### Manual Installation

1. Download this repository as ZIP
2. In Arduino IDE: Sketch → Include Library → Add .ZIP Library
3. Select the downloaded ZIP file

## Usage

```cpp
#include "MicroI2CMaster.h"

void setup() {
  // Initialize I2C
  MicroI2CMaster_begin();
}

void loop() {
  // Write to device at address 0x38
  MicroI2CMaster_start();
  MicroI2CMaster_write(0x38 << 1 | 0);  // Address + write bit
  MicroI2CMaster_write(0xAC);           // Command
  MicroI2CMaster_stop();

  delay(100);  // Wait for measurement

  // Read from device
  uint8_t data;
  MicroI2CMaster_start();
  MicroI2CMaster_write(0x38 << 1 | 1);  // Address + read bit
  MicroI2CMaster_read(&data, false);    // Read with NACK
  MicroI2CMaster_stop();

  delay(1000);  // Wait before next measurement
}
```

## Pin Configuration

By default, the library uses:

- **SDA**: PB2 (equivalent to pin 10 on Arduino UNO/Nano, pin 16 on Pro Micro)
- **SCL**: PB3 (equivalent to pin 11 on Arduino UNO/Nano, pin 14 on Pro Micro)

You can customize the pins by defining macros before including the header:

```cpp
#define MICRO_I2C_MASTER_SDA PB1  // Use PB1 for SDA
#define MICRO_I2C_MASTER_SCL PB4  // Use PB4 for SCL
#include "MicroI2CMaster.h"
```

**Note for Arduino UNO/Nano/Pro Micro users**: The library uses PBx register names (PB0 = pin 8 on UNO/Nano, pin 15 on Pro Micro). The library handles register operations directly.

## API Reference

### Functions

- `MicroI2CMaster_begin()` - Initialize I2C pins
- `MicroI2CMaster_start()` - Generate I2C start condition
- `MicroI2CMaster_stop()` - Generate I2C stop condition
- `MicroI2CMaster_write(uint8_t data)` - Write data byte, returns true if ACK received
- `MicroI2CMaster_read(uint8_t* data, bool ack)` - Read data byte, set ack to true to send ACK
- `MicroI2CMaster_scan(uint8_t* data, uint8_t count)` - Scan I2C bus and return count of found devices

### Macros

- `MICRO_I2C_MASTER_SDA` - SDA pin number (default: PB2)
- `MICRO_I2C_MASTER_SCL` - SCL pin number (default: PB3)
- `MICRO_I2C_MASTER_DELAY()` - Timing delay (default: `_delay_us(5)`)

## Examples

See the `examples/` directory for complete usage examples:

- **AHTX0**: Temperature and humidity sensor example
- **SCAN**: I2C bus scanner example
- **SSD1306**: OLED display example showing "HELLO!" message

## License

MIT License - see LICENSE file for details.

## Contributing

Pull requests and issues are welcome on GitHub.
