
typedef struct {
    volatile uint32_t SR, DR, BRR, CR1, CR2, CR3, GTPR;
} uart;

#define UART6 ((uart *) 0x40011400)
#define UART1 ((uart *) 0x40011000)
#define UART2 ((uart *) 0x40004400)

// enable UART2 via RCC APB1ENR bit 17

/*
The Tx and Rx pins are A2 and A3 respectively. These are GPIO - we need to set their modes.
*/

// write 0111 to GPIOx_AFRL select alternate function 7

#define FREQ 16000000 // CPU frequency, 16 MHz
static inline void uart_init(uart *controller, unsigned long baud) {
    uint8_t af = 7;
    uint16_t tx = 0, rx = 0;

    /* ------- USART Clock Enable ------- */
    if (controller == UART6) RCC -> APB2ENR |= BIT(5);
    if (controller == UART1) RCC -> APB2ENR |= BIT(4);
    if (controller == UART2) RCC -> APB1ENR |= BIT(17); 

    /* --------- Tx & Rx Pins ------------*/
    if (controller == UART6) tx = PIN('C', 6), rx = PIN('C', 7);
    if (controller == UART1) tx = PIN('A', 9), rx = PIN('A', 10);
    if (controller == UART2) tx = PIN('A', 2), rx = PIN('A', 3);

    gpio_set_mode(tx, GPIO_MODE_AF);
    gpio_set_af(tx, af);
    gpio_set_mode(rx, GPIO_MODE_AF);
    gpio_set_af(rx, af);

    controller -> CR1 = 0;
    controller -> BRR = FREQ/baud;
    controller -> CR1 |= BIT(13) | BIT(3) | BIT(2);

    //SR Bit 7 - Transmit data register empty
    //SR Bit 6 - Transmission Complete
    //SR Bit 5 - Received data is ready to be read
}

static inline void spin(volatile uint32_t count) {
  while (count--) (void) 0;
}

static inline int uart_read_ready(uart *controller) {
    return controller -> SR & BIT(5); // If RXNE bit is set, data is ready
}

static inline int transmission_done(uart *controller) {
    return controller -> SR & BIT(6);
}

static inline int transmit_empty(uart *controller) {
    return controller -> SR & BIT(7);
}

static inline uint8_t uart_read_byte(uart *controller) {
    return (uint8_t) (controller->DR & 255);
}

static inline void uart_write_byte(uart *controller, uint8_t byte) {
    controller->DR = byte;
    while ((transmit_empty(controller)) == 0) spin(1);
}

static inline void uart_write_buf(uart *controller, char *buf, size_t len) {
  while (len-- > 0) uart_write_byte(controller, *(uint8_t *) buf++);
}