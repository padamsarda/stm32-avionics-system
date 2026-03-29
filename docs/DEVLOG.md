this devlog was made by comprising my notes from notion
link to almost all my learning for this project :
https://www.notion.so/padam-sarda/STM32F401-2fa8f697af2780d8a9efd0637aff9324?source=copy_link

# Devlog (Key Learnings)

This is not a day-by-day log — only the important things that actually changed how I think about embedded systems.

---

## 1. What Actually Matters vs What I Learned

I ended up using maybe ~20% of what I learned in the final system.

But the unused 80% is what makes the 20% fail-proof and deterministic

---

## 2. Core Hardware Understanding

Before writing anything meaningful, I focused on how the MCU actually works:

* AHB / APB buses
* I-CODE, D-CODE, S-CODE buses
* Where parallelism actually exists (and where it doesn’t)

Key realization:
> parallelism breaks at one point and the buses - latency are inter-related

---

## 3. Interrupts 

I spent time understanding:

* Vector table + NVIC
* Exception entry (hardware stack push)
* Preemption and priority
* Fault escalation

Also looked at:

* Disassembly during debugging
* Startup code

---

## 4. Stack Behavior

* How stack grows
* What gets pushed automatically during interrupts
* High-water mark technique for stack usage

Key realization:

> Stack misuse doesn’t fail immediately rather, it fails unpredictably.

---

## 5. Volatility and Atomicity

* `volatile` and atomic behavior plays a very important role in interrupts (ISRs)

---

## 6. Clock Tree and Timing

* STM32 clock tree (PLL, prescalers)
* Timer frequency derivation
* Latency calculation

Key realization:

> If timing is wrong, everything built on top is wrong.

---

## 7. ADC + DMA Pipeline

* TRGO triggering (hardware-controlled sampling)
* DMA circular mode
* Half-transfer / full-transfer interrupts

Did multiple experiments to verify behavior.

Key realization:

> There is much more optimisation that can be done in this area, will dive deeper in future work

---

## 8. Scheduler and Worst Case Thinking

* Timer-driven scheduler
* Measured WCET using DWT
* Compared against time budget

Key realization:

> Worst case defines the system and scheduling increases determinism.

---

## 9. UART Streaming Without Blocking

* Ring buffer (HEAD / TAIL)
* ISR-driven transmit (TXE interrupt)
* Push (UART TX) vs pop (ISR) model

---

## 10. Fault Detection and Recovery

* Overrun detection (scheduler overlap)
* ADC stall detection (heartbeat method)
* One-shot recovery attempt

Key realization:

> Detect → attempt recovery → then fail cleanly with logging

---

## 11. Watchdog Design

* Logical watchdog (software health check)
* Hardware watchdog (IWDG reset)

Key realization:

> A watchdog is much more useful if tied to real system health.

---

## 12. Flash Logging Reality

* Tried erase + write → caused issues, corrupted flash
* Switched to append-only logging
* Used compact event codes instead of strings

Key realization:

> working with Flash is not the simplest thing, will dive deeper in future work

---

## 13. Final Takeaways

* Determinism > features
* Hardware offloading > CPU work
* Interrupts are not free
* Most failures are timing-related

---

This project was not only about building something complex but more about understanding how systems fail and making sure failure is controlled.
