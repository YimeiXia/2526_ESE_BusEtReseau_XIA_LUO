/*
 * rpi.c
 *
 *  Created on: Nov 27, 2025
 *      Author: Kelly
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rpi.h"


uint8_t rxData[6];
char txData[10];

//extern float K_value;
//extern uint8_t angle_value;
//
//
//bmp280_comp_param_t compensation_params;


void rpi_init(h_rpi_t *h_rpi)
{
	bmp_get_trimming_params(h_rpi->compensation_params);
	HAL_UART_Receive_IT(h_rpi->huart, rxData, 5);
}

/*
 * A appeler dans la callback de reception UART
 */
void rpi_traitement_requete(h_rpi_t *h_rpi)
{
	memset(txData, 0, sizeof(txData));

	static int size = 0;
	static uint32_t raw_temp;
	static uint32_t raw_press;
	static uint32_t comp_temp;
	static uint32_t comp_press;


	printf("Received: %c%c%c%c%c\r\n", rxData[0], rxData[1], rxData[2], rxData[3], rxData[4]);

	if (rxData[0] == 'G' && rxData[1] == 'E' && rxData[2] == 'T' && rxData[3] == '_')
	{
		switch(rxData[4])
		{
		case 'T':
		{
			get_bmp280_raw_temp(&raw_temp);
			comp_temp = bmp280_compensate_T_int32(raw_temp, h_rpi->compensation_params);
			size = sprintf(txData, "T=%06xH", (unsigned int) comp_temp);

			HAL_UART_Transmit(h_rpi->huart, (uint8_t*)txData, size, HAL_MAX_DELAY);
			break;
		}
		case 'P':
		{
			get_bmp280_raw_press(&raw_press);
			comp_press = bmp280_compensate_P_int32(raw_press, h_rpi->compensation_params);
			size = sprintf(txData, "P=%06lxH", comp_press);

			HAL_UART_Transmit(h_rpi->huart, (uint8_t*)txData, size, HAL_MAX_DELAY);
			break;
		}
		case 'K':
		{
			size = sprintf(txData, "K=%2.5f", *(h_rpi->K_value));
			HAL_UART_Transmit(h_rpi->huart, (uint8_t*)txData, size, HAL_MAX_DELAY);
			break;
		}
		case 'A':
		{
			size = sprintf(txData, "A=%d", (unsigned int)*(h_rpi->angle_value));
			HAL_UART_Transmit(h_rpi->huart, (uint8_t*)txData, size, HAL_MAX_DELAY);
			break;
		}
		default:
			printf("Unknown GET command\r\n");
			break;
		}

	}
	else if (rxData[0] == 'S' && rxData[1] == 'E' && rxData[2] == 'T' && rxData[3] == '_')
	{
		HAL_UART_Receive(h_rpi->huart, rxData, 4, HAL_MAX_DELAY);

		char val_str[10];
		sprintf(val_str, "%c%c%c%c \r\n", rxData[0], rxData[1], rxData[2], rxData[3]);
		printf("new value of K : %f\r\n", (float)(atoi(val_str)) / 100.0f);
		*(h_rpi->K_value) = (float)(atoi(val_str)) / 100.0f;

		size = sprintf(txData, "SET=OK");
		HAL_UART_Transmit(h_rpi->huart, (uint8_t*)txData, size, HAL_MAX_DELAY);
	}

	HAL_UART_Receive_IT(h_rpi->huart, rxData, 5);

}
