# Cálculos para Fio Nicromo - Tambor Cilíndrico

## Especificações do Tambor:
- **Diâmetro**: 6 cm
- **Comprimento**: 2,5 cm
- **Temperatura máxima**: 350°C
- **Fonte**: 12V/10A (120W máximo)

## Cálculos do Fio:

### 1. Área de Superfície do Tambor:
```
Perímetro = π × diâmetro = 3,14 × 6cm = 18,84 cm
Área = Perímetro × comprimento = 18,84 × 2,5 = 47,1 cm²
```

### 2. Potência Necessária para 350°C:
Para atingir 350°C em um tambor de 47 cm², precisamos de aproximadamente:
- **Potência estimada**: 80-100W
- **Densidade de potência**: ~2W/cm²

### 2.1. Considerações para Fio 0,3mm:
- **Fio mais fino**: Maior resistência por metro
- **Maior flexibilidade**: Mais fácil de enrolar
- **Menor massa**: Aquece mais rápido
- **Cuidado**: Pode quebrar se mal manuseado

### 3. Resistência do Fio:
```
P = V²/R → R = V²/P
R = 12²/90 = 144/90 = 1,6Ω
```

### 4. Comprimento do Fio:
Para fio nicromo 0,3mm:
- **Resistividade**: 1,1 Ω/m (mesma do 0,5mm)
- **Comprimento necessário**: 1,6Ω ÷ 1,1Ω/m = **1,45 metros**

### 5. Enrolamento no Tambor:
```
Perímetro do tambor = 18,84 cm
Número de voltas = 145cm ÷ 18,84cm = 7,7 voltas
Espaçamento entre voltas = 2,5cm ÷ 7,7 = 3,2mm
```

## Especificações Recomendadas:

### Fio Nicromo:
- **Diâmetro**: 0,3mm (28 AWG)
- **Comprimento**: 1,5 metros
- **Resistência**: ~1,6Ω
- **Material**: Nichrome 80 (80% Ni, 20% Cr)

### Potência:
- **Máxima**: 90W (75% da fonte)
- **Corrente máxima**: 7,5A
- **Tensão**: 12V

### Enrolamento:
- **Voltas**: 8 voltas
- **Espaçamento**: 3mm entre voltas
- **Isolamento**: Cerâmica ou mica entre voltas

## Cálculos de Segurança:

### 1. Verificação de Corrente:
```
I = V/R = 12V/1,6Ω = 7,5A ✓ (menor que 10A da fonte)
```

### 2. Verificação de Potência:
```
P = V × I = 12V × 7,5A = 90W ✓ (menor que 120W da fonte)
```

### 3. Temperatura do Fio:
```
Temperatura estimada: 350°C ✓ (objetivo atingido)
```

## Circuito Atualizado:

### MOSFET IRF3205:
- **Corrente máxima**: 7,5A (bem dentro dos 110A)
- **Tensão**: 12V (bem dentro dos 55V)
- **Dissipação**: Baixa (MOSFET totalmente saturado)

### Diodo Flyback:
- **1N4007**: 1A (suficiente para 7,5A de pico)
- **Melhor opção**: 1N5408 (3A) - mais robusta

### Resistor Gate:
- **100Ω**: Adequado para 3,3V do ESP32
- **Corrente gate**: 33mA (seguro)

## Recomendações de Montagem:

### 1. Enrolamento:
- Use isolamento cerâmico entre voltas
- Mantenha espaçamento uniforme
- Fixe as pontas bem firmes

### 2. Refratário:
- Forre o tambor com material refratário
- Use cimento refratário para fixar o fio

### 3. Proteções:
- Disjuntor de 10A na fonte
- Fusível de 8A no circuito
- Termostato de segurança (opcional)

## Teste Gradual:

### 1. Comece com PWM baixo:
- 10% → 20% → 30% → 50% → 75% → 100%
- Monitore temperatura com termopar

### 2. Ajuste fino:
- Se não atingir 350°C: aumente voltas do fio
- Se superaquecer: diminua voltas ou potência

### 3. Calibração:
- Meça resistência real do fio
- Ajuste cálculos se necessário
