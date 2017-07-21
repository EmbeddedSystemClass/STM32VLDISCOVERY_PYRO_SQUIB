#include "pyro_squib.h"
#include "stm32f1xx_hal.h"
#include "dig_pot.h"
#include "adc.h"

#define PYRO_SQUIB_NUM 4

#define PYRO_SQUIB_TIME_MIN		10
#define PYRO_SQUIB_TIME_MAX		100
#define IS_PYRO_SQUIB_TIME(__TIME__) (((__TIME__) >=PYRO_SQUIB_TIME_MIN) && ((__TIME__) <= PYRO_SQUIB_TIME_MAX))


#define PYRO_SQUIB_CURRENT_MIN	0.5
#define PYRO_SQUIB_CURRENT_MAX	5.0
#define IS_PYRO_SQUIB_CURRENT(__CURRENT__) (((__CURRENT__) >=PYRO_SQUIB_CURRENT_MIN) && ((__CURRENT__) <= PYRO_SQUIB_CURRENT_MAX))


extern TIM_HandleTypeDef htim2;
static stPyroSquib PyroSquibParam;

//enPyroSquibError PyroSquib_SetParameters(stPyroSquib *PyroSquib, uint16_t time,float current, uint8_t mask)
//{
//	HAL_StatusTypeDef hal_err=HAL_ERROR;
//	enPyroSquibError  pyr_err=PYRO_SQUIB_INCORRECT_PARAM;
//	

//	
//	
//	if(IS_PYRO_SQUIB_TIME(time))
//	{
//		PyroSquib->time=time;
//	}
//	else
//	{
//		return PYRO_SQUIB_INCORRECT_PARAM;
//	}
//	
//	if(IS_PYRO_SQUIB_CURRENT(current))
//	{
//		uint8_t pot_val=0;
//		uint8_t pyro_squib_cnt=0;
//		PyroSquib->current=current;
//		
//		for(pyro_squib_cnt=0;pyro_squib_cnt<DIG_POT_4;pyro_squib_cnt++)
//		{
//				hal_err=DigPot_SetValue(pyro_squib_cnt, pot_val);
//				if(hal_err!=HAL_OK)
//				{
//						return PYRO_SQUIB_I2C_ERR;
//				}
//		}
//	}
//	else
//	{
//		return PYRO_SQUIB_INCORRECT_PARAM;	 
//	}
//	
//	return PYRO_SQUIB_OK;
//}


enPyroSquibError PyroSquib_SetTime(stPyroSquib *PyroSquib, uint16_t time)
{
	if(IS_PYRO_SQUIB_TIME(time))
	{
		PyroSquib->time=time;
	}
	else
	{
		return PYRO_SQUIB_INCORRECT_PARAM;
	}
	
	return PYRO_SQUIB_OK;
}

enPyroSquibError PyroSquib_SetCurrent(stPyroSquib *PyroSquib,float current)
{
	HAL_StatusTypeDef hal_err=HAL_ERROR;

	if(IS_PYRO_SQUIB_CURRENT(current))
	{
		uint8_t pot_val=0;
		uint8_t pyro_squib_cnt=0;
		PyroSquib->current=current;
		
		for(pyro_squib_cnt=0;pyro_squib_cnt<DIG_POT_4;pyro_squib_cnt++)
		{
				hal_err=DigPot_SetValue(pyro_squib_cnt, pot_val);
				if(hal_err!=HAL_OK)
				{
						return PYRO_SQUIB_I2C_ERR;
				}
		}
	}
	else
	{
		return PYRO_SQUIB_INCORRECT_PARAM;	 
	}
	
	return PYRO_SQUIB_OK;	
}

enPyroSquibError PyroSquib_SetMask(stPyroSquib *PyroSquib, uint8_t mask)
{
		PyroSquib->mask=mask;
}

enPyroSquibError PyroSquib_SetKeysState(stPyroSquib *PyroSquib, enPyroSquibKeysState state)
{
		HAL_GPIO_WritePin(PIR_EN1_GPIO_Port,PIR_EN1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIR_EN2_GPIO_Port,PIR_EN2_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIR_EN3_GPIO_Port,PIR_EN3_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIR_EN4_GPIO_Port,PIR_EN4_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIR_EN5_GPIO_Port,PIR_EN5_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIR_EN6_GPIO_Port,PIR_EN6_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIR_EN7_GPIO_Port,PIR_EN7_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIR_EN8_GPIO_Port,PIR_EN8_Pin,GPIO_PIN_RESET);
	
	
		if(state==PYRO_SQUIB_KEYS_ON)
		{
				if(PyroSquib->mask&(1<<0)) HAL_GPIO_WritePin(PIR_EN1_GPIO_Port,PIR_EN1_Pin,GPIO_PIN_SET);
				if(PyroSquib->mask&(1<<1)) HAL_GPIO_WritePin(PIR_EN2_GPIO_Port,PIR_EN2_Pin,GPIO_PIN_SET);
				if(PyroSquib->mask&(1<<2)) HAL_GPIO_WritePin(PIR_EN3_GPIO_Port,PIR_EN3_Pin,GPIO_PIN_SET);
				if(PyroSquib->mask&(1<<3)) HAL_GPIO_WritePin(PIR_EN4_GPIO_Port,PIR_EN4_Pin,GPIO_PIN_SET);
				if(PyroSquib->mask&(1<<4)) HAL_GPIO_WritePin(PIR_EN5_GPIO_Port,PIR_EN5_Pin,GPIO_PIN_SET);
				if(PyroSquib->mask&(1<<5)) HAL_GPIO_WritePin(PIR_EN6_GPIO_Port,PIR_EN6_Pin,GPIO_PIN_SET);
				if(PyroSquib->mask&(1<<6)) HAL_GPIO_WritePin(PIR_EN7_GPIO_Port,PIR_EN7_Pin,GPIO_PIN_SET);
				if(PyroSquib->mask&(1<<7)) HAL_GPIO_WritePin(PIR_EN8_GPIO_Port,PIR_EN8_Pin,GPIO_PIN_SET);
		}
}

enPyroSquibError PyroSquib_Start(void)
{
	//start adc 
	
	//enable current keys
	PyroSquib_SetKeysState(&PyroSquibParam,PYRO_SQUIB_KEYS_ON);
	
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	HAL_TIM_Base_Start_IT(&htim2);
}

void PyroSquib_TimerExpired(void)
{
	//disable current keys
	PyroSquib_SetKeysState(&PyroSquibParam,PYRO_SQUIB_KEYS_OFF);
	
	//stop adc
	
}

