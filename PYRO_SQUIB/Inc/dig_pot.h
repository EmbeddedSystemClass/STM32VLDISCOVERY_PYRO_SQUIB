#ifndef DIG_POT_H
#define DIG_POT_H
#include "stm32f1xx_hal.h"


#define I2C_POT_NUM			4
//#define DIG_POT_MIN_VALUE		0
//#define DIG_POT_MIN_CURRENT	0

//#define DIG_POT_MAX_VALUE		127
//#define DIG_POT_MAX_CURRENT	127


#define DIG_POT_VALUE_MIN	0
#define DIG_POT_VALUE_MAX	127
#define IS_DIG_POT_VALUE(__VALUE__) (((__VALUE__) >=DIG_POT_VALUE_MIN) && ((__VALUE__) <= DIG_POT_VALUE_MAX))


#define DIG_POT_CURRENT_MIN	0.0
#define DIG_POT_CURRENT_MAX	5.0
#define IS_DIG_POT_CURRENT(__CURRENT__) (((__CURRENT__) >=DIG_POT_CURRENT_MIN) && ((__CURRENT__) <= DIG_POT_CURRENT_MAX))


typedef enum
{
	DIG_POT_1=0,
	DIG_POT_2=1,
	DIG_POT_3=2,
	DIG_POT_4=3,
} enDigPot;


void DigPot_Init(void);
HAL_StatusTypeDef DigPot_SetValue(enDigPot DigPot, uint8_t value);
uint8_t DigPot_CurrentToPotVal(float current);

#endif