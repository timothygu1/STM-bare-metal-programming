#include <stdbool.h>

/*
Systick is a 24-bit counter register in ARM core processors that triggers interrupts.

Systick address 0x0000 003C in vector table

16 MHz Nucleo board?

Task: Trigger interrupt each millisecond
Cycles = frequency * seconds
16E6 * 0.001 = 16000

In the RELOAD register put the appropriate calculated timer value - write to [23:0]
Clear current value - write anything to [23:0]
Program control/status register:
- Enable: Write 1 to [0]

*/
typedef struct {
  volatile uint32_t CTRL, LOAD, VAL, CALIB;
} systick;
#define SYSTICK ((systick *) 0xE000E010)

static inline void systick_init(uint32_t ticks) {
  if ((ticks-1) > 0xffffff) return; // Systick timer is 24-bit
  SYSTICK -> LOAD = ticks - 1;
  SYSTICK -> VAL = 0;
  SYSTICK -> CTRL = BIT(0) | BIT(1) | BIT(2);
  RCC -> APB2ENR |= BIT(14);
}

static volatile uint32_t s_ticks;
void SysTick_Handler(void) {
  s_ticks++;
}

// This function polls to see if the timer has elapsed
// t: expiration time, prd: period, now: current time. Return true if expired
bool timer_expired(uint32_t *t, uint32_t prd, uint32_t now) {
  if (now + prd < *t) *t = 0;
  if (*t == 0) *t = now + prd;
  if (*t > now) return false;
  *t = (now - *t) > prd ? now + prd : *t + prd;
  return true;
}
