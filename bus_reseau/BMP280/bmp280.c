/*
 * bmp280.c
 *
 *  Created on: Nov 26, 2025
 *      Author: Kelly
 */

#include "main.h"
#include "bmp280.h"

#include "i2c.h"

#define BMP_DEV_ADDRESS 0x77 << 1
#define BMP_REG_ID 0xD0

void bmp_get_id(uint8_t *id)
{
	uint8_t pdata = BMP_REG_ID;

	 HAL_I2C_Master_Transmit(&hi2c1, BMP_DEV_ADDRESS, &pdata, 1, HAL_MAX_DELAY);
	 HAL_I2C_Master_Receive(&hi2c1, BMP_DEV_ADDRESS, id, 1, HAL_MAX_DELAY);

}
