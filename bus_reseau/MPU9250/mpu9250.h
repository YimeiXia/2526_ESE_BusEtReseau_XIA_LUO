/*
 * mpu9250.h
 *
 *  Created on: Nov 27, 2025
 *      Author: Kelly
 */

#ifndef MPU9250_H_
#define MPU9250_H_

typedef struct
{
	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
} mpu9250_accel_data_t;

void mpu_get_id(uint8_t *id);

void mpu_configure();

void mpu_get_accel_data(mpu9250_accel_data_t *accel_data);

void mpu_disp_accel_data(mpu9250_accel_data_t *accel_data);



#endif /* MPU9250_H_ */
