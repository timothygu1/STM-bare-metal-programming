struct gpio {
    // Memory mapped GPIO registers
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
    // this list of registers is defined in this order, to map 1:1 to actual hardware memory layout
}

#define GPIOA ((struct gpio *) 0x40020000)
#define GPIO(bank) ((struct gpio*) 0x40020000 + (bank)*0x400)
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num)) // create 16 bit #, upper 8 bits bank, lower 8 bits num
#define PINNO(pin) (pin & 255) // get num
#define PINBANK(pin) (pin >> 8) // get bank

enum {GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG}

static inline void gpio_set_mode(struct gpio *gpio, uint16_t pin, uint8_t mode) {
    struct gpio *gpio = GPIO(PINBANK(pin)); // GPIO bank
    uint8_t n = PINNO(pin);
    gpio->MODER &= ~(3U << (n * 2)); // Clear existing setting
    gpio->MODER |= (mode & 3) << (n * 2); // Set new mode
}


