/*
 * bmp280.h
 *
 *  Created on: Nov 26, 2025
 *      Author: Kelly
 */

#ifndef BMP280_H_
#define BMP280_H_

#define BMP280_S32_t int32_t
#define BMP280_U32_t uint32_t

typedef struct
{
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;

	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
} bmp280_comp_param_t;

void bmp_get_id(uint8_t *id);
void bmp_configure();

void bmp_get_trimming_params(bmp280_comp_param_t *params);

void get_bmp280_raw_temp(uint32_t *raw_temp);
void get_bmp280_raw_press(uint32_t *raw_press);

BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T, bmp280_comp_param_t *comp_params);
BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P, bmp280_comp_param_t *comp_params);

#endif /* BMP280_H_ */
