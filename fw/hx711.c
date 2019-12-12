#include "hx711.h"
#include <nrf52.h>
#include <nrf52_bitfields.h>

#define HX711_SCK_PIN		7
#define HX711_MISO_PIN		8
//#define HX711_MOSI_PIN		4

void hx711_init() {
	NRF_SPI1->PSEL.SCK = HX711_SCK_PIN;
	NRF_SPI1->PSEL.MISO = HX711_MISO_PIN;
	//NRF_SPI1->PSEL.MOSI = HX711_MOSI_PIN; // unused
	NRF_SPI1->CONFIG |= 1 << 1; // sample on trailing edge
	NRF_SPI1->FREQUENCY = SPI_FREQUENCY_FREQUENCY_K500;
	NRF_SPI1->ENABLE = SPI_ENABLE_ENABLE_Enabled;
}

uint32_t hx711_read() {
	uint8_t rx_buf[3];
	const uint8_t c = 0xff;

	NRF_SPI1->TXD = c;
	NRF_SPI1->TXD = c;

	while (!NRF_SPI1->EVENTS_READY) ;
	NRF_SPI1->EVENTS_READY = 0;

	rx_buf[0] = NRF_SPI1->RXD;
	NRF_SPI1->TXD = c;

	while (!NRF_SPI1->EVENTS_READY) ;
	NRF_SPI1->EVENTS_READY = 0;

	rx_buf[1] = NRF_SPI1->RXD;

	while (!NRF_SPI1->EVENTS_READY) ;
	NRF_SPI1->EVENTS_READY = 0;

	rx_buf[2] = NRF_SPI1->RXD;

	uint32_t v = rx_buf[2] | (rx_buf[1] << 8) | (rx_buf[0] << 16);
	v ^= 0x800000;

	return v;
}
