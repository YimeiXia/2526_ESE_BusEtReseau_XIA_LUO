/*
 * bmp280.c
 *
 *  Created on: Nov 26, 2025
 *      Author: Kelly
 */

#include <stdio.h>

#include "main.h"
#include "bmp280.h"

#include "i2c.h"

#define BMP_DEV_ADDRESS 0x77 << 1
#define BMP_REG_ID 0xD0

#define BMP_REG_CTRL_MEAS 0xF4
#define BMP_MODE_NORMAL 0x03
#define BMP_OSRS_P_X16 0x05 << 2
#define BMP_OSRS_T_X2 0x02 << 5


void bmp_get_id(uint8_t *id)
{
	uint8_t pdata = BMP_REG_ID;

	 HAL_I2C_Master_Transmit(&hi2c1, BMP_DEV_ADDRESS, &pdata, 1, HAL_MAX_DELAY);
	 HAL_I2C_Master_Receive(&hi2c1, BMP_DEV_ADDRESS, id, 1, HAL_MAX_DELAY);
}

/*
 * Configure the BMP280 sensor with :
 * - Normal mode
 * - Pressure oversampling x16
 * - Temperature oversampling x2
 */
void bmp_configure()
{
	uint8_t pdata[2];
	uint8_t ctrl_meas_value = 0;

	pdata[0] = BMP_REG_CTRL_MEAS;
	pdata[1] = BMP_MODE_NORMAL | BMP_OSRS_P_X16 | BMP_OSRS_T_X2;

	HAL_I2C_Master_Transmit(&hi2c1, BMP_DEV_ADDRESS, pdata, 2, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, BMP_DEV_ADDRESS, &ctrl_meas_value, 1, HAL_MAX_DELAY);

	if (ctrl_meas_value != pdata[1])
	{
		printf("BMP280 configuration failed!\r\n");
	}
}
