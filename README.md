# Deterministic Fault-Tolerant Embedded System (STM32F401)
Focus was not features — but determinism, fault handling, and how the system behaves when things go wrong.

---

## System Overview

* Timer-driven scheduler (500 µs)
* ADC sampling using DMA (no CPU involvement)
* Interrupt-driven UART
* State machine: INIT → RUN → FAULT
* Dual watchdog:

  * Logical (software)
  * Hardware (IWDG)
* Persistent fault logging in flash (append-only)

---

## System Flow

<img width="1499" height="851" alt="Image" src="https://github.com/user-attachments/assets/ae2bef98-6ae9-418a-b14f-f15a980dbf4b" />

---

## Demo

▶️ Demo Video:
[demonstration video link](https://drive.google.com/file/d/1YDvpAeZ6JUo9RbYDhRiQCuaUSvlFt9lw/view?usp=sharing)

---

## Key Features

### Deterministic Scheduler

* TIM2 based (500 µs)
* No RTOS
* Single-threaded execution
* Measured using DWT cycle counter

### ADC + DMA Pipeline

* Triggered by timer (TRGO)
* Circular DMA buffer
* Zero CPU sampling overhead

### Fault Detection

* Overrun detection
  Detects scheduler overlap using `running_flag`

* ADC stall detection
  Uses DMA heartbeat (ISR-based)

### Recovery Logic

* ADC + DMA restart
* If recovery fails → system goes to FAULT

### Watchdog Design

* Logical watchdog checks system health
* Hardware watchdog (IWDG):

  * Fed only in RUN
  * Not fed in FAULT → forces reset

### Flash Logging

* Append-only (no runtime erase)
* Persistent across resets
* Logs only critical fault events

Format:
[timestamp][event_code]

---

## Timing Performance

* t_min ≈ 36 cycles
* t_max ≈ 24,000 cycles
* Budget ≈ 42,000 cycles
(more details in v3_scheduler/experiments)
→ System meets deadline comfortably

---

## Fault Behavior

System flow:

detect → recover → fail → FAULT → reset

* FAULT state:

  * Stops normal tasks
  * Logs event once
  * Waits for watchdog reset

---

## How to Run

1. Flash code to STM32F401 (NUCLEO board)
2. Open serial monitor (USART2)
3. Observe:

   * ADC data stream
   * Fault messages
   * System reset behavior

---

## Design Choices

* No RTOS (intentional)
* No MPU (kept simple)
* No flash erase during runtime
* No unnecessary abstraction

Goal was clarity + control, not complexity

---

## Notes

This project was mainly about understanding:

* how things fail
* how to detect failure early
* and how to recover safely

Not about building a big system — but building a correct one



