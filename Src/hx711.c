#include "hx711.h"
#include "main.h"

static uint32_t scale_zero_FR = 0;
static uint32_t scale_zero_FL = 0;
static uint32_t scale_zero_RR = 0;
static uint32_t scale_zero_RL = 0;

void calibrate_scale(void)
{
	scale_zero_FR = measurement(1);
	scale_zero_FL = measurement(2);
	scale_zero_RR = measurement(3);
	scale_zero_RL = measurement(4);
}

bool measure_pin(uint8_t pin)
{
	switch (pin)
	{
	case 1:
		return HAL_GPIO_ReadPin(DATA_FR_GPIO_Port, DATA_FR_Pin);
		break;
	case 2:
		return HAL_GPIO_ReadPin(DATA_FL_GPIO_Port, DATA_FL_Pin);
		break;
	case 3:
		return HAL_GPIO_ReadPin(DATA_RR_GPIO_Port, DATA_RR_Pin);
		break;
	case 4:
		return HAL_GPIO_ReadPin(DATA_RL_GPIO_Port, DATA_RL_Pin);
		break;
	default:
		return 0;
		break;
	}

}
uint32_t measurement(uint8_t measured_pin)
{

	int32_t data = 0;

	SCK_RESET;

	while (!measure_pin(measured_pin))
		;
	while (measure_pin(measured_pin))
		;
	for (uint8_t var = 24; var--;)
	{
		SCK_SET;
		if (measure_pin(measured_pin))
		{
			bit_set(data, var);
		}
		SCK_RESET;
	}

	SCK_SET;
	SCK_RESET;

	data = data ^ 0x800000;

	return data;
}

void weight_grams(uint32_t *weight_FR, uint32_t *weight_FL, uint32_t *weight_RR, uint32_t *weight_RL, uint16_t FR, uint16_t FL, uint16_t RR, uint16_t RL)
{
	*weight_FR = measurement(1);

	if (*weight_FR > scale_zero_FR)
	{
		*weight_FR = 0;
	}
	else
	{
		*weight_FR = (scale_zero_FR - *weight_FR) / FR;
	}

	*weight_FL = measurement(2);

	if (*weight_FL > scale_zero_FL)
	{
		*weight_FL = 0;
	}
	else
	{
		*weight_FL = (scale_zero_FL - *weight_FL) / FL;
	}

	*weight_RR = measurement(3);

	if (*weight_RR > scale_zero_RR)
	{
		*weight_RR = 0;
	}
	else
	{
		*weight_RR = (scale_zero_RR - *weight_RR) / RR;
	}

	*weight_RL = measurement(4);

	if (*weight_RL > scale_zero_RL)
	{
		*weight_RL = 0;
	}
	else
	{
		*weight_RL = (scale_zero_RL - *weight_RL) / RL;
	}
}

