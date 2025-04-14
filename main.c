
int main(void) {
    return 0;
}

// How does __attribute__ specifying work in C?

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

