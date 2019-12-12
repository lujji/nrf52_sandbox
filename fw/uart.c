#include "uart.h"
#include <nrf52.h>
#include <nrf52_bitfields.h>

void uart_init() {
	NRF_UART0->PSELTXD = 12;
	NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200;
	NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled;
	NRF_UART0->TASKS_STARTTX = 1;
}

void uart_write8(uint8_t c) {
	NRF_UART0->TXD = c;
	while (!NRF_UARTE0->EVENTS_TXDRDY) ;
	NRF_UARTE0->EVENTS_TXDRDY = 0;
}

void uart_write32(uint32_t v) {
	uart_write8(v);
	uart_write8(v >> 8);
	uart_write8(v >> 16);
	uart_write8(v >> 24);
}
