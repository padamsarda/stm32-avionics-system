# Requirements

## Goal
Build a deterministic and redundant multi-sensor system using STM32F401 with only bare-metal

## Features
R - D - C (showcasing redundancy , determinism and complexity)
## Redundancy
- memory protection
- watchdog
- failstate log
## Determinism
- Scheduling
    ->Fixed cycle scheduling
    ->Measured execution times
- Overtime mechanism
- Latency and jitter optimisation
## Complexity
- Analog input (ADC + DMA)
- digital input (I2C/SPI)
- state machine (INIT → RUN → FAULT)
- eeprom logging
- UART used for serial monitor display
    

## Constraints
- Bare-metal (no RTOS)
- CMSIS only
