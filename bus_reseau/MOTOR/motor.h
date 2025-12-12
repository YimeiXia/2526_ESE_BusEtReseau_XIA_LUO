/*
 * motor.h
 *
 *  Created on: Dec 12, 2025
 *      Author: Kelly
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#define MOTOR_CLOCKWISE 0x00
#define MOTOR_ANTICLOCKWISE 0x01


void motor_init(void);
void motor_turn(uint8_t angle, uint8_t direction);


#endif /* MOTOR_H_ */
