#ifndef LSM6DS3_H_
#define LSM6DS3_H_

#include <stdint.h>

void lsm6ds3_init();
void lsm6ds3_wait_data_ready();
void lsm6ds3_gyro_enable();
void lsm6ds3_accel_enable();
void lsm6ds3_accel_disable();
void lsm6ds3_accel_read(int16_t *xyz);
void lsm6ds3_gyro_read(int16_t *xyz);

uint8_t lsm6ds3_status_read();
float lsm6ds3_temp_read();

#endif /* LSM6DS3_H_ */
