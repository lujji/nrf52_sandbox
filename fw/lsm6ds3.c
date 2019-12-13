#include "lsm6ds3.h"
#include <nrf52.h>
#include <nrf52_bitfields.h>

#include <nrf_delay.h>
#include <SEGGER_RTT.h>
#define printf(...)     	do { SEGGER_RTT_printf(0, __VA_ARGS__); } while(0)

/* IO pins */
#define LSM6DS3_CS_PIN		15
#define LSM6DS3_SCK_PIN		14
#define LSM6DS3_MOSI_PIN	13
#define LSM6DS3_MISO_PIN	12

/* internal registers */
#define WHO_AM_I			0x0F

#define CTRL1_XL			0x10
#define CTRL1_XL_ODR_XL0	4
#define CTRL1_XL_FS_XL1		3
#define CTRL1_XL_FS_XL0		2

#define CTRL2_G				0x11
#define CTRL2_G_ODR_G0		4
#define CTRL2_G_FS_G1		3
#define CTRL2_G_FS_G0		2

#define STATUS_REG			0x1E
#define STATUS_REG_TDA		2
#define STATUS_REG_GDA		1
#define STATUS_REG_XLDA		0

#define OUT_TEMP_L			0x20 // 2 bytes temperature
#define OUTX_L_G			0x22 // 6 bytes gyroscope
#define OUTX_L_XL			0x28 // 6 bytes accelerometer

#define REG_READ			(1 << 7)

static uint8_t rx[8];
static uint8_t tx[8];

static void spim_trx(uint8_t count) {
	// assert count < max_buffer_size
	NRF_P0->OUT &= ~(1 << LSM6DS3_CS_PIN);
	NRF_SPIM0->TXD.MAXCNT = count;
	NRF_SPIM0->RXD.MAXCNT = count;
	NRF_SPIM0->TASKS_START = 1;

	while (!NRF_SPIM0->EVENTS_END) ;
	NRF_SPIM0->EVENTS_END = 0;
	NRF_P0->OUT |= (1 << LSM6DS3_CS_PIN);
}

static void write_reg(uint8_t reg, uint8_t val) {
	tx[0] = reg;
	tx[1] = val;
	spim_trx(2);
}

static uint8_t read_reg(uint8_t reg) {
	tx[0] = reg | REG_READ;
	tx[1] = 0xff;
	spim_trx(2);
	return rx[1];
}

/*
 * SPI configuration:
 *
 * SDx,SCx -> GND
 * SA0 -> MISO
 * SDA -> MOSI
 * SCL -> SCK
 * CS -> CS
 */
void lsm6ds3_init() {
	/* IO pins */
	NRF_SPIM0->PSEL.SCK = LSM6DS3_SCK_PIN;
	NRF_SPIM0->PSEL.MOSI = LSM6DS3_MOSI_PIN;
	NRF_SPIM0->PSEL.MISO = LSM6DS3_MISO_PIN;
	NRF_P0->DIR |= (1 << LSM6DS3_CS_PIN);
	NRF_P0->OUT |= (1 << LSM6DS3_CS_PIN);

	NRF_SPIM0->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_K500;
	//NRF_SPIM0->CONFIG |= 1 << 2; // CPOL = 1

	/* DMA pointers */
	NRF_SPIM0->RXD.PTR = rx;
	NRF_SPIM0->TXD.PTR = tx;

	NRF_SPIM0->ENABLE = SPIM_ENABLE_ENABLE_Enabled;
}

int lsm6ds3_check_id() {
	return read_reg(WHO_AM_I) == 0x69;
}

void lsm6ds3_gyro_enable() {
	uint8_t reg = (1 << CTRL2_G_ODR_G0); // 12.5Hz low power mode
	write_reg(CTRL2_G, reg);
}

void lsm6ds3_accel_enable() {
	uint8_t reg = (1 << CTRL1_XL_ODR_XL0); // 12.5Hz low power mode
	write_reg(CTRL1_XL, reg);
}

void lsm6ds3_accel_disable() {
	write_reg(CTRL1_XL, 0);
}

uint8_t lsm6ds3_status_read() {
	return read_reg(STATUS_REG);
}

float lsm6ds3_temp_read() {
	tx[0] = OUT_TEMP_L | REG_READ;
	spim_trx(3);
	int16_t temp = rx[1] | (rx[2] << 8);

	/* 25 degC offset w/ 16 LSB per degC */
	float t = 25.0 + temp / 16.0;
	return t;
}

void lsm6ds3_wait_data_ready() {
	while (!(lsm6ds3_status_read() & ((1 << STATUS_REG_XLDA) | ((1 << STATUS_REG_GDA))))) ;
}

void lsm6ds3_accel_read(int16_t *xyz) {
	tx[0] = OUTX_L_XL | REG_READ;
	spim_trx(7);

	xyz[0] = rx[1] | (rx[2] << 8);
	xyz[1] = rx[3] | (rx[4] << 8);
	xyz[2] = rx[5] | (rx[6] << 8);
}

void lsm6ds3_gyro_read(int16_t *xyz) {
	tx[0] = OUTX_L_G | REG_READ;
	spim_trx(7);

	xyz[0] = rx[1] | (rx[2] << 8);
	xyz[1] = rx[3] | (rx[4] << 8);
	xyz[2] = rx[5] | (rx[6] << 8);
}
