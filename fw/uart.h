#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void uart_init();
void uart_write8(uint8_t c);
void uart_write32(uint32_t v);

#endif /* UART_H_ */
