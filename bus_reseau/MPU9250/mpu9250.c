/*
 * mpu9250.c
 *
 *  Created on: Nov 27, 2025
 *      Author: Kelly
 */

#include <stdio.h>

#include "main.h"
#include "mpu9250.h"

#include "i2c.h"

#define MPU_DEV_ADDRESS 0x68 << 1
#define MPU_REG_WHO_AM_I 0x75

#define MPU_REG_ACCEL_CONFIG 0x1C
#define MPU_ACCEL_FS_2G 0x00
#define MPU_ACCEL_FP_2G_LSB 16384.0f

#define MPU_REG_ACCEL_XOUT_H 0x3B



void mpu_get_id(uint8_t *id)
{
	uint8_t pdata = MPU_REG_WHO_AM_I;

	HAL_I2C_Master_Transmit(&hi2c1, MPU_DEV_ADDRESS, &pdata, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, MPU_DEV_ADDRESS, id, 1, HAL_MAX_DELAY);
}

void mpu_configure()
{
	uint8_t pdata[2];
	uint8_t accel_config_value = 0;

	pdata[0] = MPU_REG_ACCEL_CONFIG;
	pdata[1] = MPU_ACCEL_FS_2G << 3;

	HAL_I2C_Master_Transmit(&hi2c1, MPU_DEV_ADDRESS, pdata, 2, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, MPU_DEV_ADDRESS, &accel_config_value, 1, HAL_MAX_DELAY);

	if (accel_config_value != pdata[1])
	{
		printf("MPU9250 configuration failed!\r\n");
	}
}

void mpu_get_accel_data(mpu9250_accel_data_t *accel_data)
{
	uint8_t tData = MPU_REG_ACCEL_XOUT_H;
	uint8_t rData[6];

	HAL_I2C_Master_Transmit(&hi2c1, MPU_DEV_ADDRESS, &tData, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, MPU_DEV_ADDRESS, rData, 6, HAL_MAX_DELAY);

	accel_data->accel_x = (int16_t)(rData[0] << 8) | rData[1];
	accel_data->accel_y = (int16_t)(rData[2] << 8) | rData[3];
	accel_data->accel_z = (int16_t)(rData[4] << 8) | rData[5];
	printf("Accel X: %d, Y: %d, Z: %d\r\n", accel_data->accel_x, accel_data->accel_y, accel_data->accel_z);
}

void mpu_disp_accel_data(mpu9250_accel_data_t *accel_data)
{
	float ax = accel_data->accel_x / MPU_ACCEL_FP_2G_LSB;
	float ay = accel_data->accel_y / MPU_ACCEL_FP_2G_LSB;
	float az = accel_data->accel_z / MPU_ACCEL_FP_2G_LSB;

	printf("Accel (g) X: %.3f, Y: %.3f, Z: %.3f\r\n", ax, ay, az);
}

