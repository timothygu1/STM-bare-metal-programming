#define GPIO(bank) ((gpio *) (0x40020000 + 0x400 * (bank)))
#define BIT(x) (1UL << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)
#define RCC ((rcc *) 0x40023800)

typedef struct {
    // Memory mapped GPIO registers
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
    // this list of registers is defined in this order, to map 1:1 to actual hardware memory layout
} gpio;

typedef struct{
    volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR, RESERVED0[2], 
    APB1RSTR, APB2RSTR, RESERVED1[2], AHB1ENR, AHB2ENR, RESERVED2[2], APB1ENR, APB2ENR, 
    RESERVED3[2], AHB1LPENR, AHB2LPENR, RESERVED4[2], APB1LPENR, APB2LPENR, 
    RESERVED5[2], BDCR, CSR, RESERVED6[2], SSCGR, PLLI2SCFGR, RESERVED7, DCKCFGR;
} rcc;

enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
    gpio *gp = GPIO(PINBANK(pin)); // GPIO bank
    int n = PINNO(pin);
    RCC->AHB1ENR |= BIT(PINBANK(pin)); // Enable GPIO clock
    gp->MODER &= ~(3U << (n * 2)); // Clear existing setting
    gp->MODER |= (mode & 3) << (n * 2); // Set new mode
}

static inline void gpio_write(uint16_t pin, bool val) {
  gpio *gp = GPIO(PINBANK(pin));
  gp->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
}

static inline void gpio_set_af(uint16_t pin, uint8_t func) {
    gpio *gp = GPIO(PINBANK(pin));
    int n = PINNO(pin);
    // n >> 3 = 0 for pins 0-7, 1 for pins 8-15
    gp -> AFR[n >> 3] &= ~(15UL << ((4*(n & 7))))        // Clear register
    gp -> AFR[n >> 3] |= ((func & 15UL) << (4*(n & 7))); // Write new value
}