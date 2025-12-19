/*
 * rpi.h
 *
 *  Created on: Nov 27, 2025
 *      Author: Kelly
 */

#ifndef RPI_H_
#define RPI_H_

#include "usart.h"
#include "bmp280.h"

typedef struct
{
	UART_HandleTypeDef *huart;
	float * K_value;
	uint8_t * angle_value;

	bmp280_comp_param_t *compensation_params;

} h_rpi_t;

void rpi_init(h_rpi_t *h_rpi);
void rpi_traitement_requete(h_rpi_t *h_rpi);



#endif /* RPI_H_ */
