/*
 * menu.h
 *
 *  Created on: Mar 17, 2018
 *      Author: raoul
 */

#ifndef MENU_H_
#define MENU_H_

#include "stm32f1xx_hal.h"
#include "stdbool.h"

#define SCREEN_REFRESH_RATE 500
#define SCREEN_TOUCH_RATE 500

typedef enum
{
	BEGIN_SCREEN,
	MAIN_MENU,
	CALIBRATE,
	CALIBRATE_FR,
	CALIBRATE_FL,
	CALIBRATE_RR,
	CALIBRATE_RL,
	DATA_CENTER_1,
	DATA_CENTER_2,
	DATA_CENTER_3,

} menumode_t;

void menu_handler(uint16_t *battery_voltage);
void touchscreen_pressed();


#endif /* MENU_H_ */
