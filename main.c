#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
//#include "gpio.h"

struct gpio {
    // Memory mapped GPIO registers
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
    // this list of registers is defined in this order, to map 1:1 to actual hardware memory layout
};
#define GPIO(bank) ((struct gpio *) (0x40020000 + 0x400 * (bank)))
#define BIT(x) (1UL << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)

struct rcc {
    volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR, RESERVED0[2], 
    APB1RSTR, APB2RSTR, RESERVED1[2], AHB1ENR, AHB2ENR, RESERVED2[2], APB1ENR, APB2ENR, 
    RESERVED3[2], AHB1LPENR, AHB2LPENR, RESERVED4[2], APB1LPENR, APB2LPENR, 
    RESERVED5[2], BDCR, CSR, RESERVED6[2], SSCGR, PLLI2SCFGR, RESERVED7, DCKCFGR;
};
#define RCC ((struct rcc *) 0x40023800)

enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
    struct gpio *gpio = GPIO(PINBANK(pin)); // GPIO bank
    int n = PINNO(pin);
    gpio->MODER &= ~(3U << (n * 2)); // Clear existing setting
    gpio->MODER |= (mode & 3) << (n * 2); // Set new mode
}

static inline void gpio_write(uint16_t pin, bool val) {
  struct gpio *gpio = GPIO(PINBANK(pin));
  gpio->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
}

static inline void spin(volatile uint32_t count) {
  while (count--) asm("nop");
}

int main(void) {
    uint16_t led = PIN('A', 5); // User LED
    RCC->AHB1ENR |= BIT(PINBANK(led));
    gpio_set_mode(led, GPIO_MODE_OUTPUT);
    for (;;) {
        gpio_write(led, false);
        spin(999999);
        gpio_write(led, true);
        spin(999999);
    }
    return 0;
}

// Boot function
__attribute__((naked, noreturn)) void _reset(void) {
  // memset .bss to zero, and copy .data section to RAM region  
  extern long _sbss, _ebss, _sdata, _edata, _sidata;
  for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
  for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;

  main(); // Call main()
  for(;;) (void) 0; // Infinite loop in the case if main() returns
}

extern void _estack(void);  // Defined in link.ld

// Specify a vector table:
// Contains 16 standard and 91 STM32-specific interrupt handlers
// Array of 16 + 91 pointers to functions with no return value, no arguments
__attribute__((section(".vectors"))) void (*const tab[16 + 91])(void) = {
  _estack, _reset
};

