/*
 * motor.c
 *
 *  Created on: Dec 12, 2025
 *      Author: Kelly
 */
#include <stdint.h>
#include <stdio.h>
#include "motor.h"
#include "can.h"


void motor_init()
{
	if (HAL_CAN_Start(&hcan1)!= HAL_OK)
		printf("Error in CAN start \r\n");
}

CAN_TxHeaderTypeDef txHeader = {
		.StdId = 0x60,
		.ExtId = 0x00,
		.IDE = CAN_ID_STD,
		.RTR = CAN_RTR_DATA,
		.DLC = 2,
		.TransmitGlobalTime = DISABLE
};

uint32_t txMailbox;

void motor_turn(uint8_t direction, uint8_t angle)
{

	uint8_t txData[3];
	txData[0] = direction;
	txData[1] = angle;
	txData[2] = 0x01;

	if (HAL_CAN_AddTxMessage(&hcan1, &txHeader, txData, &txMailbox) != HAL_OK)
		printf("Sending in can failed \r\n");

}
