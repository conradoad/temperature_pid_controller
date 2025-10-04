# Temperature PID Controller - ESP32

A temperature control system using ESP32 with PID algorithm for precise temperature control.

## Project Overview

This project implements a closed-loop temperature control system using:

- **Sensor**: Thermocouple + MAX6675 module (SPI communication)
- **Actuator**: Nichrome wire controlled via MOSFET IRF3205 (PWM)
- **Controller**: ESP32 with PID algorithm
- **Interface**: Web server for monitoring and configuration

## Hardware Components

- ESP32 development board
- Thermocouple sensor
- MAX6675 thermocouple-to-digital converter
- MOSFET IRF3205
- Nichrome heating wire
- Supporting electronics (resistors, capacitors, etc.)

## Software Features

- SPI communication with MAX6675
- PWM control for heating element
- PID control algorithm
- Web server interface
- Real-time monitoring
- Parameter configuration

## Building and Flashing

### Prerequisites

- ESP-IDF v5.5-rc1 installed
- ESP32 development board
- USB cable for programming

### Build Commands

```bash
# Set up ESP-IDF environment
source ~/esp/v5.5-rc1/esp-idf/export.sh

# Configure project
idf.py menuconfig

# Build project
idf.py build

# Flash to ESP32
idf.py -p /dev/ttyUSB0 flash

# Monitor serial output
idf.py -p /dev/ttyUSB0 monitor
```

### Quick Build and Flash

```bash
# Build and flash in one command
idf.py -p /dev/ttyUSB0 build flash monitor
```

## Project Structure

```
temperature_pid_controller/
├── main/
│   ├── temperature_controller_main.c
│   └── CMakeLists.txt
├── CMakeLists.txt
├── README.md
└── sdkconfig.ci
```

## Development Status

- [x] Basic project structure
- [x] Compilation test
- [ ] SPI communication with MAX6675
- [ ] PWM control implementation
- [ ] PID algorithm
- [ ] Web server interface
- [ ] Complete system integration

## Next Steps

1. Implement SPI communication with MAX6675
2. Add PWM control for MOSFET
3. Implement PID control algorithm
4. Create web server interface
5. Add configuration management
6. Testing and calibration

## License

This project is open source. See individual component licenses for details.