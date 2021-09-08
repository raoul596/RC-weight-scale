/*
 * menu.c
 *
 *  Created on: Mar 17, 2018
 *      Author: raoul
 */

#include "main.h"
#include "menu.h"
#include "tm_stm32_ili9341.h"
#include "ILI9341_Touchscreen.h"
#include "stdbool.h"
#include "hx711.h"
#include "time.h"
#include "flash_ee.h"

bool touchpad_pressed = false;
bool show_save = false, old_show_save;
static uint8_t touch_timer = NO_TIMER;

static menumode_t menu_mode = BEGIN_SCREEN;
static uint8_t old_screen = 1;
uint32_t weight_FR, weight_FL, weight_RR, weight_RL;
uint32_t old_weight_FR, old_weight_FL, old_weight_RR, old_weight_RL;
uint32_t weight_right, weight_left, weight_rear, weight_front;
uint32_t old_weight_right, old_weight_left, old_weight_rear, old_weight_front;
uint32_t percent_right, percent_left, percent_rear, percent_front;
uint32_t old_percent_right, old_percent_left, old_percent_rear, old_percent_front;
uint16_t old_flash_read_FR, old_flash_read_FL, old_flash_read_RR, old_flash_read_RL;
uint16_t old_battery_voltage;
char buff[10];
static uint16_t x = 0;
static uint16_t y = 0;

static uint8_t refresh_timer = NO_TIMER;
bool refresh = false;

flash_ee_t flash_read;

void menu_handler(uint16_t *battery_voltage) // called every ms
{
	if (timer_elapsed(refresh_timer))
	{
		refresh = true;
		//printf("Timer elapsed\r\n");
	}

	if (touchpad_pressed)
	{
		switch (menu_mode)
		{
		case BEGIN_SCREEN:

			calibrate_scale();

			menu_mode = MAIN_MENU;
			touchpad_pressed = false;

			if (ReadStruct2Flash(&flash_read))
			{
				printf("failed to read values from FLASH\r\n");
			}

			break;

		case MAIN_MENU:
			if (x > 26 && x < 294 && y > 27 && y < 107)
			{
				menu_mode = CALIBRATE;
			}

			if (x > 26 && x < 294 && y > 113 && y < 213)
			{
				menu_mode = DATA_CENTER_1;
			}

			touchpad_pressed = false;
			break;

		case CALIBRATE:
			if (x > 1 && x < 30 && y > 1 && y < 50)
			{
				menu_mode = MAIN_MENU;
			}
			if (x > 165 && x < 310 && y > 40 && y < 130)
			{
				menu_mode = CALIBRATE_FR;
			}
			if (x > 10 && x < 155 && y > 40 && y < 130)
			{
				menu_mode = CALIBRATE_FL;
			}
			if (x > 140 && x < 310 && y > 140 && y < 230)
			{
				menu_mode = CALIBRATE_RR;
			}
			if (x > 10 && x < 155 && y > 140 && y < 230)
			{
				menu_mode = CALIBRATE_RL;
			}

			touchpad_pressed = false;
			break;

		case CALIBRATE_FR:
			if (x > 1 && x < 30 && y > 1 && y < 50)
			{
				menu_mode = CALIBRATE;
				timer_free(&refresh_timer);
				refresh_timer = NO_TIMER;
			}
			if (x > 10 && x < 70 && y > 170 && y < 230) //+10
			{
				flash_read.FR += 10;
				show_save = true;
			}
			if (x > 80 && x < 140 && y > 170 && y < 230) //+1
			{
				flash_read.FR++;
				show_save = true;
			}
			if (x > 180 && x < 240 && y > 170 && y < 230) //-1
			{
				flash_read.FR--;
				show_save = true;
			}
			if (x > 250 && x < 310 && y > 170 && y < 230) //-10
			{
				flash_read.FR -= 10;
				show_save = true;
			}
			if (show_save && x > 250 && x < 310 && y > 40 && y < 75) //+10
			{
				if (WriteStruct2Flash(flash_read))
				{
					printf("failed to write values to FLASH\r\n");
				}

				else
				{
					show_save = false;
				}
			}

			old_flash_read_FR = flash_read.FR;
			touchpad_pressed = false;
			break;

		case CALIBRATE_FL:
			if (x > 1 && x < 30 && y > 1 && y < 50)
			{
				menu_mode = CALIBRATE;
				timer_free(&refresh_timer);
				refresh_timer = NO_TIMER;
			}
			if (x > 10 && x < 70 && y > 170 && y < 230) //+10
			{
				flash_read.FL += 10;
				show_save = true;
			}
			if (x > 80 && x < 140 && y > 170 && y < 230) //+1
			{
				flash_read.FL++;
				show_save = true;
			}
			if (x > 180 && x < 240 && y > 170 && y < 230) //-1
			{
				flash_read.FL--;
				show_save = true;
			}
			if (x > 250 && x < 310 && y > 170 && y < 230) //-10
			{
				flash_read.FL -= 10;
				show_save = true;
			}
			if (show_save && x > 250 && x < 310 && y > 40 && y < 75) //+10
			{
				if (WriteStruct2Flash(flash_read))
				{
					printf("failed to write values to FLASH\r\n");
				}

				else
				{
					show_save = false;
				}
			}

			old_flash_read_FR = flash_read.FL;
			touchpad_pressed = false;
			break;

		case CALIBRATE_RR:
			if (x > 1 && x < 30 && y > 1 && y < 50)
			{
				menu_mode = CALIBRATE;
				timer_free(&refresh_timer);
				refresh_timer = NO_TIMER;
			}
			if (x > 10 && x < 70 && y > 170 && y < 230) //+10
			{
				flash_read.RR += 10;
				show_save = true;
			}
			if (x > 80 && x < 140 && y > 170 && y < 230) //+1
			{
				flash_read.RR++;
				show_save = true;
			}
			if (x > 180 && x < 240 && y > 170 && y < 230) //-1
			{
				flash_read.RR--;
				show_save = true;
			}
			if (x > 250 && x < 310 && y > 170 && y < 230) //-10
			{
				flash_read.RR -= 10;
				show_save = true;
			}
			if (show_save && x > 250 && x < 310 && y > 40 && y < 75) //+10
			{
				if (WriteStruct2Flash(flash_read))
				{
					printf("failed to write values to FLASH\r\n");
				}

				else
				{
					show_save = false;
				}
			}

			old_flash_read_FR = flash_read.RR;
			touchpad_pressed = false;
			break;

		case CALIBRATE_RL:
			if (x > 1 && x < 30 && y > 1 && y < 50)
			{
				menu_mode = CALIBRATE;
				timer_free(&refresh_timer);
				refresh_timer = NO_TIMER;
			}
			if (x > 10 && x < 70 && y > 170 && y < 230) //+10
			{
				flash_read.RL += 10;
				show_save = true;
			}
			if (x > 80 && x < 140 && y > 170 && y < 230) //+1
			{
				flash_read.RL++;
				show_save = true;
			}
			if (x > 180 && x < 240 && y > 170 && y < 230) //-1
			{
				flash_read.RL--;
				show_save = true;
			}
			if (x > 250 && x < 310 && y > 170 && y < 230) //-10
			{
				flash_read.RL -= 10;
				show_save = true;
			}
			if (show_save && x > 250 && x < 310 && y > 40 && y < 75) //+10
			{
				if (WriteStruct2Flash(flash_read))
				{
					printf("failed to write values to FLASH\r\n");
				}

				else
				{
					show_save = false;
				}
			}

			old_flash_read_FR = flash_read.RL;
			touchpad_pressed = false;
			break;

		case DATA_CENTER_1:
			if (x > 1 && x < 50 && y > 1 && y < 60)
			{
				menu_mode = MAIN_MENU;
				timer_free(&refresh_timer);
				refresh_timer = NO_TIMER;
			}
			if (x > 1 && x < 400 && y > 50 && y < 300)
			{
				menu_mode = DATA_CENTER_2;
			}

			touchpad_pressed = false;

			break;

		case DATA_CENTER_2:
			if (x > 1 && x < 50 && y > 1 && y < 60)
			{
				menu_mode = MAIN_MENU;
				timer_free(&refresh_timer);
				refresh_timer = NO_TIMER;
			}
			if (x > 1 && x < 400 && y > 50 && y < 300)
			{
				menu_mode = DATA_CENTER_3;
			}

			touchpad_pressed = false;

			break;

		case DATA_CENTER_3:
			if (x > 1 && x < 50 && y > 1 && y < 60)
			{
				menu_mode = MAIN_MENU;
				timer_free(&refresh_timer);
				refresh_timer = NO_TIMER;
			}
			if (x > 1 && x < 400 && y > 50 && y < 300)
			{
				menu_mode = DATA_CENTER_1;
			}

			touchpad_pressed = false;

			break;
		}
	}

	if (menu_mode != old_screen || refresh)
	{
		old_screen = menu_mode;

		switch (menu_mode)
		{

		case BEGIN_SCREEN:
			//Rotate LCD for 90 degrees
			TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_1);
			TM_ILI9341_Fill(BLACK);
			// Wrote 2 strings on the top of the screen
			TM_ILI9341_Puts(100, 10, "Raoul Ursum", &TM_Font_11x18, WHITE, BLACK);
			TM_ILI9341_Puts(80, 40, "RC weight scale", &TM_Font_11x18, WHITE, BLACK);
			TM_ILI9341_Puts(55, 200, "Make sure the scales are empty", &TM_Font_7x10, WHITE, BLACK);
			TM_ILI9341_Puts(90, 220, "Touch to continue...", &TM_Font_7x10, WHITE, BLACK);

			break;

		case MAIN_MENU:
			TM_ILI9341_Fill(BLACK);

			TM_ILI9341_DrawFilledRectangle(26, 27, 294, 107, DARKGREEN);
			TM_ILI9341_Puts(105, 57, "Calibrate", &TM_Font_11x18, WHITE, DARKGREEN);

			TM_ILI9341_DrawFilledRectangle(26, 133, 294, 213, DARKGREEN);
			TM_ILI9341_Puts(100, 163, "Data center", &TM_Font_11x18, WHITE, DARKGREEN);

			break;

		case CALIBRATE:
			TM_ILI9341_Fill(BLACK);

			TM_ILI9341_Puts(2, 10, "back", &TM_Font_7x10, WHITE, BLACK);
			TM_ILI9341_Puts(105, 5, "Calibrate:", &TM_Font_11x18, WHITE, BLACK);
			TM_ILI9341_DrawLine(0, 26, 320, 30, DARKGREEN);

			TM_ILI9341_DrawFilledRectangle(165, 40, 310, 130, DARKGREEN);
			TM_ILI9341_Puts(230, 75, "FR", &TM_Font_11x18, WHITE, DARKGREEN);

			TM_ILI9341_DrawFilledRectangle(10, 40, 155, 130, DARKGREEN);
			TM_ILI9341_Puts(75, 75, "FL", &TM_Font_11x18, WHITE, DARKGREEN);

			TM_ILI9341_DrawFilledRectangle(165, 140, 310, 230, DARKGREEN);
			TM_ILI9341_Puts(230, 178, "RR", &TM_Font_11x18, WHITE, DARKGREEN);

			TM_ILI9341_DrawFilledRectangle(10, 140, 155, 230, DARKGREEN);
			TM_ILI9341_Puts(72, 178, "RL", &TM_Font_11x18, WHITE, DARKGREEN);

			break;

		case CALIBRATE_FR:
			if (refresh)
			{
				old_weight_FR = weight_FR;

				weight_grams(&weight_FR, &weight_FL, &weight_RR, &weight_RL, flash_read.FR, flash_read.FL, flash_read.RR, flash_read.RL);

				if (old_weight_FR != weight_FR)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_FR);
					TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_FR);
					TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, WHITE, BLACK);
				}

				if (old_battery_voltage != *battery_voltage)
				{
					if (LOW_BATTERY_WARNING > *battery_voltage)
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, RED, BLACK);
					}
					else
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);
					}

				}
				old_battery_voltage = *battery_voltage;

				if (show_save)
				{
					TM_ILI9341_DrawFilledRectangle(250, 40, 310, 75, DARKGREEN);
					TM_ILI9341_Puts(260, 50, "save", &TM_Font_11x18, WHITE, DARKGREEN);
					//printf("Save displayed\r\n");
				}
				else
				{
					TM_ILI9341_DrawFilledRectangle(250, 40, 310, 75, BLACK);
				}

			}
			else
			{
				TM_ILI9341_Fill(BLACK);

				TM_ILI9341_Puts(2, 10, "back", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(105, 5, "Front Right:", &TM_Font_11x18, WHITE, BLACK);
				TM_ILI9341_DrawFilledRectangle(0, 26, 320, 30, DARKGREEN);

				TM_ILI9341_DrawRectangle(277, 11, 280, 15, WHITE);
				TM_ILI9341_DrawRectangle(280, 7, 318, 19, WHITE);
				snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
				TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);

				TM_ILI9341_DrawFilledRectangle(10, 170, 70, 230, DARKGREEN);
				TM_ILI9341_Puts(30, 190, "--", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(80, 170, 140, 230, DARKGREEN);
				TM_ILI9341_Puts(105, 190, "-", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(180, 170, 240, 230, DARKGREEN);
				TM_ILI9341_Puts(205, 190, "+", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(250, 170, 310, 230, DARKGREEN);
				TM_ILI9341_Puts(270, 190, "++", &TM_Font_11x18, WHITE, DARKGREEN);

				snprintf(buff, sizeof(buff), "%ld", weight_FR);
				TM_ILI9341_Puts(0, 40, "Grams:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, WHITE, BLACK);
			}

			if (refresh_timer == NO_TIMER)
				refresh_timer = timer_get();

			timer_start(refresh_timer, SCREEN_REFRESH_RATE, NULL);
			break;

		case CALIBRATE_FL:
			if (refresh)
			{
				old_weight_FL = weight_FL;

				weight_grams(&weight_FR, &weight_FL, &weight_RR, &weight_RL, flash_read.FR, flash_read.FL, flash_read.RR, flash_read.RL);

				if (old_weight_FL != weight_FL)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_FL);
					TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_FL);
					TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, WHITE, BLACK);
				}

				if (old_battery_voltage != *battery_voltage)
				{
					if (LOW_BATTERY_WARNING > *battery_voltage)
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, RED, BLACK);
					}
					else
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);
					}

				}
				old_battery_voltage = *battery_voltage;

				if (show_save)
				{
					TM_ILI9341_DrawFilledRectangle(250, 40, 310, 75, DARKGREEN);
					TM_ILI9341_Puts(260, 50, "save", &TM_Font_11x18, WHITE, DARKGREEN);
				}
				else
				{
					TM_ILI9341_DrawFilledRectangle(250, 40, 310, 75, BLACK);
				}
			}
			else
			{
				TM_ILI9341_Fill(BLACK);

				TM_ILI9341_Puts(2, 10, "back", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(105, 5, "Front Left:", &TM_Font_11x18, WHITE, BLACK);
				TM_ILI9341_DrawFilledRectangle(0, 26, 320, 30, DARKGREEN);

				TM_ILI9341_DrawRectangle(277, 11, 280, 15, WHITE);
				TM_ILI9341_DrawRectangle(280, 7, 318, 19, WHITE);
				snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
				TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);

				TM_ILI9341_DrawFilledRectangle(10, 170, 70, 230, DARKGREEN);
				TM_ILI9341_Puts(30, 190, "--", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(80, 170, 140, 230, DARKGREEN);
				TM_ILI9341_Puts(105, 190, "-", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(180, 170, 240, 230, DARKGREEN);
				TM_ILI9341_Puts(205, 190, "+", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(250, 170, 310, 230, DARKGREEN);
				TM_ILI9341_Puts(270, 190, "++", &TM_Font_11x18, WHITE, DARKGREEN);

				snprintf(buff, sizeof(buff), "%ld", weight_FL);
				TM_ILI9341_Puts(0, 40, "Grams:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, WHITE, BLACK);
			}

			if (refresh_timer == NO_TIMER)
				refresh_timer = timer_get();

			timer_start(refresh_timer, SCREEN_REFRESH_RATE, NULL);
			break;

		case CALIBRATE_RR:
			if (refresh)
			{
				old_weight_RR = weight_RR;

				weight_grams(&weight_FR, &weight_FL, &weight_RR, &weight_RL, flash_read.FR, flash_read.FL, flash_read.RR, flash_read.RL);

				if (old_weight_RR != weight_RR)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_RR);
					TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_RR);
					TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, WHITE, BLACK);
				}

				if (old_battery_voltage != *battery_voltage)
				{
					if (LOW_BATTERY_WARNING > *battery_voltage)
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, RED, BLACK);
					}
					else
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);
					}

				}
				old_battery_voltage = *battery_voltage;

				if (show_save)
				{
					TM_ILI9341_DrawFilledRectangle(250, 40, 310, 75, DARKGREEN);
					TM_ILI9341_Puts(260, 50, "save", &TM_Font_11x18, WHITE, DARKGREEN);
				}
				else
				{
					TM_ILI9341_DrawFilledRectangle(250, 40, 310, 75, BLACK);
				}
			}
			else
			{
				TM_ILI9341_Fill(BLACK);

				TM_ILI9341_Puts(2, 10, "back", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(105, 5, "Rear Right:", &TM_Font_11x18, WHITE, BLACK);
				TM_ILI9341_DrawFilledRectangle(0, 26, 320, 30, DARKGREEN);

				TM_ILI9341_DrawRectangle(277, 11, 280, 15, WHITE);
				TM_ILI9341_DrawRectangle(280, 7, 318, 19, WHITE);
				snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
				TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);

				TM_ILI9341_DrawFilledRectangle(10, 170, 70, 230, DARKGREEN);
				TM_ILI9341_Puts(30, 190, "--", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(80, 170, 140, 230, DARKGREEN);
				TM_ILI9341_Puts(105, 190, "-", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(180, 170, 240, 230, DARKGREEN);
				TM_ILI9341_Puts(205, 190, "+", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(250, 170, 310, 230, DARKGREEN);
				TM_ILI9341_Puts(270, 190, "++", &TM_Font_11x18, WHITE, DARKGREEN);

				snprintf(buff, sizeof(buff), "%ld", weight_RR);
				TM_ILI9341_Puts(0, 40, "Grams:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, WHITE, BLACK);
			}

			if (refresh_timer == NO_TIMER)
				refresh_timer = timer_get();

			timer_start(refresh_timer, SCREEN_REFRESH_RATE, NULL);
			break;

		case CALIBRATE_RL:
			if (refresh)
			{
				old_weight_RL = weight_RL;

				weight_grams(&weight_FR, &weight_FL, &weight_RR, &weight_RL, flash_read.FR, flash_read.FL, flash_read.RR, flash_read.RL);

				if (old_weight_RL != weight_RL)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_RL);
					TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_RL);
					TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, WHITE, BLACK);
				}

				if (old_battery_voltage != *battery_voltage)
				{
					if (LOW_BATTERY_WARNING > *battery_voltage)
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, RED, BLACK);
					}
					else
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);
					}

				}
				old_battery_voltage = *battery_voltage;

				if (show_save)
				{
					TM_ILI9341_DrawFilledRectangle(250, 40, 310, 75, DARKGREEN);
					TM_ILI9341_Puts(260, 50, "save", &TM_Font_11x18, WHITE, DARKGREEN);
				}
				else
				{
					TM_ILI9341_DrawFilledRectangle(250, 40, 310, 75, BLACK);
				}
			}
			else
			{
				TM_ILI9341_Fill(BLACK);

				TM_ILI9341_Puts(2, 10, "back", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(105, 5, "Rear Left:", &TM_Font_11x18, WHITE, BLACK);
				TM_ILI9341_DrawFilledRectangle(0, 26, 320, 30, DARKGREEN);

				TM_ILI9341_DrawRectangle(277, 11, 280, 15, WHITE);
				TM_ILI9341_DrawRectangle(280, 7, 318, 19, WHITE);
				snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
				TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);

				TM_ILI9341_DrawFilledRectangle(10, 170, 70, 230, DARKGREEN);
				TM_ILI9341_Puts(30, 190, "--", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(80, 170, 140, 230, DARKGREEN);
				TM_ILI9341_Puts(105, 190, "-", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(180, 170, 240, 230, DARKGREEN);
				TM_ILI9341_Puts(205, 190, "+", &TM_Font_11x18, WHITE, DARKGREEN);
				TM_ILI9341_DrawFilledRectangle(250, 170, 310, 230, DARKGREEN);
				TM_ILI9341_Puts(270, 190, "++", &TM_Font_11x18, WHITE, DARKGREEN);

				snprintf(buff, sizeof(buff), "%ld", weight_RL);
				TM_ILI9341_Puts(0, 40, "Grams:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(5, 55, buff, &TM_Font_11x18, WHITE, BLACK);
			}

			if (refresh_timer == NO_TIMER)
				refresh_timer = timer_get();

			timer_start(refresh_timer, SCREEN_REFRESH_RATE, NULL);
			break;

		case DATA_CENTER_1:

			if (refresh)
			{
				old_weight_FR = weight_FR;
				old_weight_FL = weight_FL;
				old_weight_RR = weight_RR;
				old_weight_RL = weight_RL;
				//printf("refresh screen\r\n");
//				weight_FR = weight_grams();
//				weight_FL = weight_FR;
//				weight_RR = weight_FR;
//				weight_RL = weight_FR;

				weight_grams(&weight_FR, &weight_FL, &weight_RR, &weight_RL, flash_read.FR, flash_read.FL, flash_read.RR, flash_read.RL);

				if (old_weight_FR != weight_FR)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_FR);
					TM_ILI9341_Puts(10, 55, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_FR);
					TM_ILI9341_Puts(10, 55, buff, &TM_Font_11x18, WHITE, BLACK);
				}
				if (old_weight_FL != weight_FL)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_FL);
					TM_ILI9341_Puts(10, 215, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_FL);
					TM_ILI9341_Puts(10, 215, buff, &TM_Font_11x18, WHITE, BLACK);
				}
				if (old_weight_RR != weight_RR)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_RR);
					TM_ILI9341_Puts(235, 55, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_RR);
					TM_ILI9341_Puts(235, 55, buff, &TM_Font_11x18, WHITE, BLACK);
				}
				if (old_weight_RL != weight_RL)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_RL);
					TM_ILI9341_Puts(235, 215, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_RL);
					TM_ILI9341_Puts(235, 215, buff, &TM_Font_11x18, WHITE, BLACK);
				}

				if (old_battery_voltage != *battery_voltage)
				{
					if (LOW_BATTERY_WARNING > *battery_voltage)
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, RED, BLACK);
					}
					else
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);
					}

				}
				old_battery_voltage = *battery_voltage;
			}

			else
			{
				//printf("build screen\r\n");
				TM_ILI9341_Fill(BLACK);
				TM_ILI9341_Puts(2, 10, "back", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(115, 5, "Weight:", &TM_Font_11x18, WHITE, BLACK);
				TM_ILI9341_DrawFilledRectangle(0, 26, 320, 30, DARKGREEN);

				TM_ILI9341_DrawRectangle(277, 11, 280, 15, WHITE);
				TM_ILI9341_DrawRectangle(280, 7, 318, 19, WHITE);
				snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
				TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_FR);
				TM_ILI9341_Puts(5, 40, "Front right:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(10, 55, buff, &TM_Font_11x18, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_FL);
				TM_ILI9341_Puts(5, 200, "Front left:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(10, 215, buff, &TM_Font_11x18, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_RR);
				TM_ILI9341_Puts(235, 40, "Rear right:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(235, 55, buff, &TM_Font_11x18, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_RL);
				TM_ILI9341_Puts(235, 200, "Rear left:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(235, 215, buff, &TM_Font_11x18, WHITE, BLACK);
			}

			if (refresh_timer == NO_TIMER)
				refresh_timer = timer_get();

			timer_start(refresh_timer, SCREEN_REFRESH_RATE, NULL);
			break;
		case DATA_CENTER_2:

			if (refresh)
			{
				old_weight_FR = weight_FR;
				old_weight_FL = weight_FL;
				old_weight_RR = weight_RR;
				old_weight_RL = weight_RL;

//				weight_FR = weight_grams();
//				weight_FL = weight_FR;
//				weight_RR = weight_FR;
//				weight_RL = weight_FR;

				weight_grams(&weight_FR, &weight_FL, &weight_RR, &weight_RL, flash_read.FR, flash_read.FL, flash_read.RR, flash_read.RL);

				if (old_weight_FR != weight_FR)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_FR);
					TM_ILI9341_Puts(10, 55, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_FR);
					TM_ILI9341_Puts(10, 55, buff, &TM_Font_11x18, WHITE, BLACK);
				}
				if (old_weight_FL != weight_FL)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_FL);
					TM_ILI9341_Puts(10, 215, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_FL);
					TM_ILI9341_Puts(10, 215, buff, &TM_Font_11x18, WHITE, BLACK);
				}
				if (old_weight_RR != weight_RR)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_RR);
					TM_ILI9341_Puts(235, 55, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_RR);
					TM_ILI9341_Puts(235, 55, buff, &TM_Font_11x18, WHITE, BLACK);
				}
				if (old_weight_RL != weight_RL)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_RL);
					TM_ILI9341_Puts(235, 215, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_RL);
					TM_ILI9341_Puts(235, 215, buff, &TM_Font_11x18, WHITE, BLACK);
				}

				weight_right = weight_FR + weight_RR;
				weight_left = weight_FL + weight_RL;
				weight_rear = weight_RR + weight_RL;
				weight_front = weight_FR + weight_FR;

				old_percent_right = percent_right;
				old_percent_left = percent_left;
				old_percent_rear = percent_rear;
				old_percent_front = percent_front;

				percent_right = 1000 * weight_right / (weight_right + weight_left);
				percent_left = 1000 * weight_left / (weight_right + weight_left);
				percent_rear = 1000 * weight_rear / (weight_front + weight_rear);
				percent_front = 1000 * weight_front / (weight_front + weight_rear);

				if (old_percent_right != percent_right)
				{
					snprintf(buff, sizeof(buff), "%ld.%ld%%", old_percent_right / 10, old_percent_right % 10);
					TM_ILI9341_Puts(135, 55, buff, &TM_Font_7x10, WHITE, BLACK);
					snprintf(buff, sizeof(buff), "%ld.%ld%%", percent_right / 10, percent_right % 10);
					TM_ILI9341_Puts(135, 55, buff, &TM_Font_7x10, WHITE, BLACK);
				}

				if (old_percent_left != percent_left)
				{
					snprintf(buff, sizeof(buff), "%ld.%ld%%", old_percent_left / 10, old_percent_left % 10);
					TM_ILI9341_Puts(135, 215, buff, &TM_Font_7x10, WHITE, BLACK);
					snprintf(buff, sizeof(buff), "%ld.%ld%%", percent_left / 10, percent_left % 10);
					TM_ILI9341_Puts(135, 215, buff, &TM_Font_7x10, WHITE, BLACK);
				}
				if (old_percent_front != percent_front)
				{
					snprintf(buff, sizeof(buff), "%ld.%ld%%", old_percent_front / 10, old_percent_front % 10);
					TM_ILI9341_Puts(5, 135, buff, &TM_Font_7x10, WHITE, BLACK);
					snprintf(buff, sizeof(buff), "%ld.%ld%%", percent_front / 10, percent_front % 10);
					TM_ILI9341_Puts(5, 135, buff, &TM_Font_7x10, WHITE, BLACK);
				}
				if (old_percent_rear != percent_rear)
				{
					snprintf(buff, sizeof(buff), "%ld.%ld%%", old_percent_rear / 10, old_percent_rear % 10);
					TM_ILI9341_Puts(235, 135, buff, &TM_Font_7x10, WHITE, BLACK);
					snprintf(buff, sizeof(buff), "%ld.%ld%%", percent_rear / 10, percent_rear % 10);
					TM_ILI9341_Puts(235, 135, buff, &TM_Font_7x10, WHITE, BLACK);
				}

				if (old_battery_voltage != *battery_voltage)
				{
					if (LOW_BATTERY_WARNING > *battery_voltage)
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, RED, BLACK);
					}
					else
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);
					}

				}
				old_battery_voltage = *battery_voltage;
			}

			else
			{
				weight_right = weight_FR + weight_RR;
				weight_left = weight_FL + weight_RL;
				weight_rear = weight_RR + weight_RL;
				weight_front = weight_FR + weight_FL;

				percent_right = 1000 * weight_right / (weight_right + weight_left);
				percent_left = 1000 * weight_left / (weight_right + weight_left);
				percent_rear = 1000 * weight_rear / (weight_front + weight_rear);
				percent_front = 1000 * weight_front / (weight_front + weight_rear);

				TM_ILI9341_Fill(BLACK);
				TM_ILI9341_Puts(2, 10, "back", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(90, 5, "Percentage:", &TM_Font_11x18, WHITE, BLACK);
				TM_ILI9341_DrawFilledRectangle(0, 26, 320, 30, DARKGREEN);

				TM_ILI9341_DrawRectangle(277, 11, 280, 15, WHITE);
				TM_ILI9341_DrawRectangle(280, 7, 318, 19, WHITE);
				snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
				TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_FR);
				TM_ILI9341_Puts(5, 40, "Front right:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(10, 55, buff, &TM_Font_11x18, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_FL);
				TM_ILI9341_Puts(5, 200, "Front left:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(10, 215, buff, &TM_Font_11x18, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_RR);
				TM_ILI9341_Puts(235, 40, "Rear right:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(235, 55, buff, &TM_Font_11x18, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_RL);
				TM_ILI9341_Puts(235, 200, "Rear left:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(235, 215, buff, &TM_Font_11x18, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld.%ld%%", percent_right / 10, percent_right % 10);
				TM_ILI9341_Puts(120, 40, "Right weight:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(135, 55, buff, &TM_Font_7x10, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld.%ld%%", percent_left / 10, percent_left % 10);
				TM_ILI9341_Puts(120, 200, "Left weight:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(135, 215, buff, &TM_Font_7x10, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld.%ld%%", percent_front / 10, percent_front % 10);
				TM_ILI9341_Puts(5, 120, "Front weight:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(5, 135, buff, &TM_Font_7x10, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld.%ld%%", percent_rear / 10, percent_rear % 10);
				TM_ILI9341_Puts(235, 120, "Rear weight:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(235, 135, buff, &TM_Font_7x10, WHITE, BLACK);

			}

			if (refresh_timer == NO_TIMER)
				refresh_timer = timer_get();

			timer_start(refresh_timer, SCREEN_REFRESH_RATE, NULL);
			break;

		case DATA_CENTER_3:

			if (refresh)
			{
				old_weight_FR = weight_FR;
				old_weight_FL = weight_FL;
				old_weight_RR = weight_RR;
				old_weight_RL = weight_RL;

//				weight_FR = weight_grams();
//				weight_FL = weight_FR;
//				weight_RR = weight_FR;
//				weight_RL = weight_FR;

				weight_grams(&weight_FR, &weight_FL, &weight_RR, &weight_RL, flash_read.FR, flash_read.FL, flash_read.RR, flash_read.RL);

				if (old_weight_FR != weight_FR)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_FR);
					TM_ILI9341_Puts(10, 55, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_FR);
					TM_ILI9341_Puts(10, 55, buff, &TM_Font_11x18, WHITE, BLACK);
				}
				if (old_weight_FL != weight_FL)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_FL);
					TM_ILI9341_Puts(10, 215, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_FL);
					TM_ILI9341_Puts(10, 215, buff, &TM_Font_11x18, WHITE, BLACK);
				}
				if (old_weight_RR != weight_RR)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_RR);
					TM_ILI9341_Puts(235, 55, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_RR);
					TM_ILI9341_Puts(235, 55, buff, &TM_Font_11x18, WHITE, BLACK);
				}
				if (old_weight_RL != weight_RL)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_RL);
					TM_ILI9341_Puts(235, 215, buff, &TM_Font_11x18, BLACK, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_RL);
					TM_ILI9341_Puts(235, 215, buff, &TM_Font_11x18, WHITE, BLACK);
				}

				old_weight_right = weight_right;
				old_weight_left = weight_left;
				old_weight_front = weight_front;
				old_weight_rear = weight_rear;

				weight_right = weight_FR + weight_RR;
				weight_left = weight_FL + weight_RL;
				weight_front = weight_FR + weight_FR;
				weight_rear = weight_RR + weight_RL;

				if (old_weight_right != weight_right)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_right);
					TM_ILI9341_Puts(135, 55, buff, &TM_Font_7x10, WHITE, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_right);
					TM_ILI9341_Puts(135, 55, buff, &TM_Font_7x10, WHITE, BLACK);
				}
				if (old_weight_left != weight_left)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_left);
					TM_ILI9341_Puts(135, 215, buff, &TM_Font_7x10, WHITE, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_left);
					TM_ILI9341_Puts(135, 215, buff, &TM_Font_7x10, WHITE, BLACK);
				}
				if (old_weight_front != weight_front)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_front);
					TM_ILI9341_Puts(5, 135, buff, &TM_Font_7x10, WHITE, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_front);
					TM_ILI9341_Puts(5, 135, buff, &TM_Font_7x10, WHITE, BLACK);
				}
				if (old_weight_rear != weight_rear)
				{
					snprintf(buff, sizeof(buff), "%ld", old_weight_rear);
					TM_ILI9341_Puts(235, 135, buff, &TM_Font_7x10, WHITE, BLACK);
					snprintf(buff, sizeof(buff), "%ld", weight_rear);
					TM_ILI9341_Puts(235, 135, buff, &TM_Font_7x10, WHITE, BLACK);
				}

				if (old_battery_voltage != *battery_voltage)
				{
					if (LOW_BATTERY_WARNING > *battery_voltage)
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, RED, BLACK);
					}
					else
					{
						snprintf(buff, sizeof(buff), "%d,%02dV", old_battery_voltage / 1000, (old_battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, BLACK, BLACK);
						snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
						TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);
					}

				}
				old_battery_voltage = *battery_voltage;
			}
			else
			{
				TM_ILI9341_Fill(BLACK);
				TM_ILI9341_Puts(2, 10, "back", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(85, 5, "Distribution:", &TM_Font_11x18, WHITE, BLACK);
				TM_ILI9341_DrawFilledRectangle(0, 26, 320, 30, DARKGREEN);

				TM_ILI9341_DrawRectangle(277, 11, 280, 15, WHITE);
				TM_ILI9341_DrawRectangle(280, 7, 318, 19, WHITE);
				snprintf(buff, sizeof(buff), "%d,%02dV", *battery_voltage / 1000, (*battery_voltage % 1000) / 10);
				TM_ILI9341_Puts(282, 9, buff, &TM_Font_7x10, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_FR);
				TM_ILI9341_Puts(5, 40, "Front right:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(10, 55, buff, &TM_Font_11x18, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_FL);
				TM_ILI9341_Puts(5, 200, "Front left:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(10, 215, buff, &TM_Font_11x18, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_RR);
				TM_ILI9341_Puts(235, 40, "Rear right:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(235, 55, buff, &TM_Font_11x18, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_RL);
				TM_ILI9341_Puts(235, 200, "Rear left:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(235, 215, buff, &TM_Font_11x18, WHITE, BLACK);

				weight_right = weight_FR + weight_RR;
				weight_left = weight_FL + weight_RL;
				weight_front = weight_FR + weight_FR;
				weight_rear = weight_RR + weight_RL;

				snprintf(buff, sizeof(buff), "%ld", weight_right);
				TM_ILI9341_Puts(120, 40, "Right weight:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(135, 55, buff, &TM_Font_7x10, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_left);
				TM_ILI9341_Puts(120, 200, "Left weight:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(135, 215, buff, &TM_Font_7x10, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_front);
				TM_ILI9341_Puts(5, 120, "Front weight:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(5, 135, buff, &TM_Font_7x10, WHITE, BLACK);

				snprintf(buff, sizeof(buff), "%ld", weight_rear);
				TM_ILI9341_Puts(235, 120, "Rear weight:", &TM_Font_7x10, WHITE, BLACK);
				TM_ILI9341_Puts(235, 135, buff, &TM_Font_7x10, WHITE, BLACK);
			}

			if (refresh_timer == NO_TIMER)
				refresh_timer = timer_get();

			timer_start(refresh_timer, SCREEN_REFRESH_RATE, NULL);
			break;
		}
		refresh = false;
	}

}

void touchscreen_pressed()
{
	if (!TOUCHED && timer_elapsed(touch_timer))
	{
		timer_start(touch_timer, SCREEN_TOUCH_RATE, NULL);
		touchpad_pressed = true;
		TP_Read_Coordinates(&x, &y);
		//printf("x= %d\r\n", x);
		//printf("y= %d\r\n", y);
	}

	if (touch_timer == NO_TIMER)
	{
		touch_timer = timer_get();
		timer_start(touch_timer, SCREEN_TOUCH_RATE, NULL);
	}

}

