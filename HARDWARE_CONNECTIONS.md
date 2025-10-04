# Hardware Connections - MAX6675 Temperature Sensor

## ESP32 to MAX6675 Connections

Para testar a leitura do sensor MAX6675, conecte os seguintes pinos:

### SPI Connections
| ESP32 Pin | MAX6675 Pin | Description |
|-----------|-------------|-------------|
| GPIO19    | DO (Data Out) | MISO - Master In, Slave Out |
| GPIO23    | DI (Data In)  | MOSI - Master Out, Slave In |
| GPIO18    | CLK (Clock)   | SPI Clock |
| GPIO5     | CS (Chip Select) | Chip Select |

### Power Connections
| ESP32 Pin | MAX6675 Pin | Description |
|-----------|-------------|-------------|
| 3.3V      | VCC         | Power Supply |
| GND       | GND         | Ground |

### Thermocouple Connections
| MAX6675 Pin | Thermocouple Wire |
|------------|-------------------|
| T+         | Positive (Red)    |
| T-         | Negative (Black)  |

## Expected Behavior

When properly connected, the ESP32 should:

1. **Initialize successfully**:
   ```
   I (xxxx) MAX6675: MAX6675 initialized successfully
   I (xxxx) MAX6675: SPI Host: 2, CS Pin: 5, Clock: 1000000 Hz
   ```

2. **Read temperature every second**:
   ```
   I (xxxx) TEMP_CONTROLLER: Reading #1: Temperature = 25.50°C
   I (xxxx) TEMP_CONTROLLER: Reading #2: Temperature = 25.75°C
   ```

3. **Show error if thermocouple not connected**:
   ```
   W (xxxx) TEMP_CONTROLLER: Reading #X: Thermocouple not connected!
   ```

## Troubleshooting

### Common Issues:

1. **"Failed to initialize MAX6675"**
   - Check SPI connections (MISO, MOSI, CLK, CS)
   - Verify power connections (3.3V, GND)
   - Ensure no pin conflicts

2. **"Thermocouple not connected!"**
   - Check thermocouple connections to T+ and T-
   - Verify thermocouple is working
   - Check for loose connections

3. **No temperature readings**
   - Verify all connections
   - Check if MAX6675 is getting power
   - Ensure SPI pins are not used by other peripherals

## Testing Without Hardware

If you don't have the MAX6675 connected yet, the system will show:
```
E (xxxx) TEMP_CONTROLLER: Failed to initialize MAX6675: [error code]
E (xxxx) TEMP_CONTROLLER: Please check SPI connections:
E (xxxx) TEMP_CONTROLLER: - MISO: GPIO19
E (xxxx) TEMP_CONTROLLER: - MOSI: GPIO23  
E (xxxx) TEMP_CONTROLLER: - CLK:  GPIO18
E (xxxx) TEMP_CONTROLLER: - CS:   GPIO5
```

This is normal behavior when the hardware is not connected.
