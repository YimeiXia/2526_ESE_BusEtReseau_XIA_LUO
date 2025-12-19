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
#include "usart.h"
#include "bmp280.h"

uint8_t rxData[6];
char txData[10];

extern float K_value;
extern uint8_t angle_value;


bmp280_comp_param_t compensation_params;
uint32_t raw_temp;
uint32_t raw_press;
uint32_t comp_temp;
uint32_t comp_press;


void rpi_init()
{
	bmp_get_trimming_params(&compensation_params);
	HAL_UART_Receive_IT(&huart1, rxData, 5);
}

/*
 * A appeler dans la callback de reception UART
 */
void rpi_traitement_requete()
{
	memset(txData, 0, sizeof(txData));

	int size = 0;

	printf("Received: %c%c%c%c%c\r\n", rxData[0], rxData[1], rxData[2], rxData[3], rxData[4]);

	if (rxData[0] == 'G' && rxData[1] == 'E' && rxData[2] == 'T' && rxData[3] == '_')
	{
		switch(rxData[4])
		{
		case 'T':
		{
			get_bmp280_raw_temp(&raw_temp);
			comp_temp = bmp280_compensate_T_int32(raw_temp, &compensation_params);
			size = sprintf(txData, "T=%06xH", (unsigned int) comp_temp);

			HAL_UART_Transmit(&huart1, (uint8_t*)txData, size, HAL_MAX_DELAY);
			break;
		}
		case 'P':
		{
			get_bmp280_raw_press(&raw_press);
			comp_press = bmp280_compensate_P_int32(raw_press, &compensation_params);
			size = sprintf(txData, "P=%06lxH", comp_press);

			HAL_UART_Transmit(&huart1, (uint8_t*)txData, size, HAL_MAX_DELAY);
			break;
		}
		case 'K':
		{
			size = sprintf(txData, "K=%2.5f", K_value);
			HAL_UART_Transmit(&huart1, (uint8_t*)txData, size, HAL_MAX_DELAY);
			break;
		}
		case 'A':
		{
			size = sprintf(txData, "A=%d", (unsigned int)angle_value);
			HAL_UART_Transmit(&huart1, (uint8_t*)txData, size, HAL_MAX_DELAY);
			break;
		}
		default:
			printf("Unknown GET command\r\n");
			break;
		}

	}
	else if (rxData[0] == 'S' && rxData[1] == 'E' && rxData[2] == 'T' && rxData[3] == '_')
	{
		HAL_UART_Receive(&huart1, rxData, 4, HAL_MAX_DELAY);

		char val_str[10];
		sprintf(val_str, "%c%c%c%c \r\n", rxData[0], rxData[1], rxData[2], rxData[3]);
		printf("new value of K : %f\r\n", (float)(atoi(val_str)) / 100.0f);
		K_value = (float)(atoi(val_str)) / 100.0f;

		size = sprintf(txData, "SET=OK");
		HAL_UART_Transmit(&huart1, (uint8_t*)txData, size, HAL_MAX_DELAY);
	}

	HAL_UART_Receive_IT(&huart1, rxData, 5);

}
