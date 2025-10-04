# MOSFET Circuit Design - IRF3205

## Circuito para Fonte 12V/10A

### Componentes Necessários:
- **MOSFET IRF3205** (N-Channel, 55V, 110A)
- **Resistor Pull-down** 10kΩ (R1)
- **Resistor Gate** 100Ω (R2) 
- **Diodo Flyback** 1N4007 (D1)
- **Fio Nicromo** (resistência ~1-5Ω)
- **Fonte 12V/10A**

### Conexões:

```
ESP32 GPIO2 ──[R2:100Ω]── Gate IRF3205
                    │
                   [R1:10kΩ]── GND

Fonte 12V+ ──[Fio Nicromo]── Drain IRF3205
                    │
                   [D1:1N4007]── Fonte 12V-
                    │
                  Source IRF3205 ── Fonte 12V-

ESP32 GND ── Fonte 12V-
```

### Especificações:
- **PWM Frequency**: 1 kHz
- **Resolution**: 12-bit (0-4095)
- **Max Power**: 120W (12V × 10A)
- **Gate Voltage**: 3.3V (ESP32)
- **MOSFET Vgs(th)**: 2-4V

### Cálculos para Tambor 6cm × 2,5cm:
- **Resistência Nicromo**: R = V²/P = 12²/90 = 1.6Ω
- **Corrente Máxima**: I = V/R = 12/1.6 = 7.5A ✓
- **Potência Máxima**: P = V×I = 12×7.5 = 90W
- **Fio necessário**: 1.5m de nicromo 0.3mm
- **Enrolamento**: 8 voltas no tambor

### Proteções:
- **Diodo Flyback**: Protege contra tensão reversa
- **Resistor Pull-down**: Garante OFF quando ESP32 desconectado
- **Resistor Gate**: Limita corrente de gate
