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

#define BMP_REG_TRIM_START 0x88
#define BMP_REG_TRIM_LENGTH 24

#define BMP_REG_TEMP_START 0xFA
#define BMP_REG_TEMP_LENGTH 3

#define BMP_REG_PRESS_START 0xF7
#define BMP_REG_PRESS_LENGTH 3

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

void bmp_get_trimming_params(bmp280_comp_param_t *params)
{
	uint8_t tData = BMP_REG_TRIM_START;
	uint8_t rData[BMP_REG_TRIM_LENGTH];

	HAL_I2C_Master_Transmit(&hi2c1, BMP_DEV_ADDRESS, &tData, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, BMP_DEV_ADDRESS, rData, BMP_REG_TRIM_LENGTH, HAL_MAX_DELAY);

	for (int i = 0; i < 24; i++)
	{
		printf("Trim param %d : %x \r\n", i, rData[i]);
	}

	params->dig_T1 = (uint16_t)(rData[1] << 8) | rData[0];
	params->dig_T2 = (int16_t)(rData[3] << 8) | rData[2];
	params->dig_T3 = (int16_t)(rData[5] << 8) | rData[4];
	params->dig_P1 = (uint16_t)(rData[7] << 8) | rData[6];
	params->dig_P2 = (int16_t)(rData[9] << 8) | rData[8];
	params->dig_P3 = (int16_t)(rData[11] << 8) | rData[10];
	params->dig_P4 = (int16_t)(rData[13] << 8) | rData[12];
	params->dig_P5 = (int16_t)(rData[15] << 8) | rData[14];
	params->dig_P6 = (int16_t)(rData[17] << 8) | rData[16];
	params->dig_P7 = (int16_t)(rData[19] << 8) | rData[18];
	params->dig_P8 = (int16_t)(rData[21] << 8) | rData[20];
	params->dig_P9 = (int16_t)(rData[23] << 8) | rData[22];
}

void get_bmp280_raw_temp(uint32_t *raw_temp)
{

	uint8_t tData = BMP_REG_TEMP_START;
	uint8_t rData[BMP_REG_TEMP_LENGTH];

	HAL_I2C_Master_Transmit(&hi2c1, BMP_DEV_ADDRESS, &tData, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, BMP_DEV_ADDRESS, rData, BMP_REG_TEMP_LENGTH, HAL_MAX_DELAY);

	*raw_temp = (uint32_t)(rData[0] << 12) | (rData[1] << 4) | (rData[2] >> 4);
}

void get_bmp280_raw_press(uint32_t *raw_press)
{

	uint8_t tData = BMP_REG_PRESS_START;
	uint8_t rData[BMP_REG_PRESS_LENGTH];

	HAL_I2C_Master_Transmit(&hi2c1, BMP_DEV_ADDRESS, &tData, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, BMP_DEV_ADDRESS, rData, BMP_REG_PRESS_LENGTH, HAL_MAX_DELAY);

	*raw_press = (uint32_t)(rData[0] << 12) | (rData[1] << 4) | (rData[2] >> 4);
}


// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t t_fine;
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T, bmp280_comp_param_t *comp_params)
{
	uint16_t dig_T1 = comp_params->dig_T1;
	int16_t dig_T2 = comp_params->dig_T2;
	int16_t dig_T3 = comp_params->dig_T3;

	BMP280_S32_t var1, var2, T;
	var1  = ((((adc_T>>3) - ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
	var2  = (((((adc_T>>4) - ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)dig_T1))) >> 12) * ((BMP280_S32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T  = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P, bmp280_comp_param_t *comp_params)
{
	uint16_t dig_P1 = comp_params->dig_P1;
	int16_t dig_P2 = comp_params->dig_P2;
	int16_t dig_P3 = comp_params->dig_P3;
	int16_t dig_P4 = comp_params->dig_P4;
	int16_t dig_P5 = comp_params->dig_P5;
	int16_t dig_P6 = comp_params->dig_P6;
	int16_t dig_P7 = comp_params->dig_P7;
	int16_t dig_P8 = comp_params->dig_P8;
	int16_t dig_P9 = comp_params->dig_P9;

	BMP280_S32_t var1, var2;
	BMP280_U32_t p;
	var1 = (((BMP280_S32_t)t_fine)>>1) - (BMP280_S32_t)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((BMP280_S32_t)dig_P6);
	var2 = var2 + ((var1*((BMP280_S32_t)dig_P5))<<1);
	var2 = (var2>>2)+(((BMP280_S32_t)dig_P4)<<16);
	var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((BMP280_S32_t)dig_P2) * var1)>>1))>>18;
	var1 =((((32768+var1))*((BMP280_S32_t)dig_P1))>>15);
	if   (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = (((BMP280_U32_t)(((BMP280_S32_t)1048576)-adc_P)-(var2>>12)))*3125;
	if   (p < 0x80000000)
	{
		p = (p << 1) / ((BMP280_U32_t)var1);
	}
	else
	{
		p = (p / (BMP280_U32_t)var1) * 2;
	}
	var1 = (((BMP280_S32_t)dig_P9) * ((BMP280_S32_t)(((p>>3) * (p>>3))>>13)))>>12;
	var2 = (((BMP280_S32_t)(p>>2)) * ((BMP280_S32_t)dig_P8))>>13;
	p = (BMP280_U32_t)((BMP280_S32_t)p + ((var1 + var2 + dig_P7) >> 4));
	return p;
}
