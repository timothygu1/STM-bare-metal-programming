# 🛠️ Bare-Metal STM32F401RE Practice

This repository contains minimal bare-metal examples for the STM32F401RE Nucleo board, written in C without any vendor HAL or CMSIS abstractions. The goal is to better understand how the microcontroller works at the register level by implementing peripheral drivers (GPIO, UART, SysTick, etc.) from scratch.

---

## 🧠 Purpose

This project serves as a learning tool for:
- Understanding the ARM Cortex-M4 architecture
- Writing startup code (reset vector, vector table)
- Initializing and using GPIO, SysTick timer, UART, and other peripherals manually
- Developing reusable drivers in a minimal embedded C environment

---

## 🚀 Getting Started

### Prerequisites
- STM32F401RE Nucleo Board
- ARM GCC toolchain (`arm-none-eabi-gcc`)
- ST-Link
- GNU Make

### Build and Flash
```bash
make flash
cu -l /dev/ttyACM0 -s 115200 # for connecting to UART serial monitor
```

### Reference Manual:
https://www.st.com/content/ccc/resource/technical/document/reference_manual/5d/b1/ef/b2/a1/66/40/80/DM00096844.pdf/files/DM00096844.pdf/jcr:content/translations/en.DM00096844.pdf

### STM32F401RE MCU Datasheet:
https://www.st.com/resource/en/datasheet/stm32f401re.pdf

