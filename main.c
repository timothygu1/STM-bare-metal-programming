#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include "./include/gpio.h"
#include "./include/systick.h"
#include "./include/uart.h"

int main(void) {
    uint16_t led = PIN('A', 5); // User LED
    systick_init(16000000 / 1000);
    gpio_set_mode(led, GPIO_MODE_OUTPUT);

    uint32_t timer, period = 1000;

    uart_init(UART2, 115200);
    for (;;) {
      if (timer_expired(&timer, period, s_ticks)) {
        static bool on;
        gpio_write(led, on);
        on = !on;
        uart_write_buf(UART2, "hi\r\n", 4);
      }
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
  _estack, _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTick_Handler};

