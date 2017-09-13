#include "pyro_squib.h"
#include "stm32f1xx_hal.h"
#include "dig_pot.h"
#include "adc.h"
#include "cfg_info.h"

#include "FreeRTOS.h"
#include "task.h"

//#define PYRO_SQUIB_NUM 4

//#define PYRO_SQUIB_TIME_MIN		10
//#define PYRO_SQUIB_TIME_MAX		500
//#define IS_PYRO_SQUIB_TIME(__TIME__) (((__TIME__) >=PYRO_SQUIB_TIME_MIN) && ((__TIME__) <= PYRO_SQUIB_TIME_MAX))



extern TIM_HandleTypeDef htim2;
extern sConfigInfo configInfo;	

uint8_t pyro_pulse_counter=0;

stPyroSquib *PyroSquibParam=&configInfo.PyroSquibParams;//={100,0.7,0.7,0.7,0.7,255,PYRO_SQUIB_OK,PYRO_SQUIB_STOP};

void PyroSquib_Timer_Start(void);
void PyroSquib_Timer_Stop(void);

//enPyroSquibError PyroSquib_SetTime(uint16_t time)
//{
//	if(IS_PYRO_SQUIB_TIME(time))
//	{
//		PyroSquibParam->time=time;
//	}
//	else
//	{
//		return PYRO_SQUIB_INCORRECT_PARAM;
//	}
//	
//	return PYRO_SQUIB_OK;
//}

//enPyroSquibError PyroSquib_SetPeriod(uint16_t period)
//{
//	if(IS_PYRO_SQUIB_PERIOD(period))
//	{
//		PyroSquibParam->period=period;
//	}
//	else
//	{
//		return PYRO_SQUIB_INCORRECT_PARAM;
//	}
//	
//	return PYRO_SQUIB_OK;
//}

//enPyroSquibError PyroSquib_SetPulseTime(uint16_t pulse_time)
//{
//	if(IS_PYRO_SQUIB_PULSE_TIME(pulse_time))
//	{
//		PyroSquibParam->pulse_time=pulse_time;
//	}
//	else
//	{
//		return PYRO_SQUIB_INCORRECT_PARAM;
//	}
//	
//	return PYRO_SQUIB_OK;
//}

//enPyroSquibError PyroSquib_SetPulsesNum(uint16_t pulses_num)
//{
//	if(IS_PYRO_SQUIB_PULSES_NUM(pulses_num))
//	{
//		PyroSquibParam->pulses_num=pulses_num;
//	}
//	else
//	{
//		return PYRO_SQUIB_INCORRECT_PARAM;
//	}
//	
//	return PYRO_SQUIB_OK;
//}

enPyroSquibError PyroSquib_SetCurrent(enPyroSquibNums PyroSquib, float current)
{
	HAL_StatusTypeDef hal_err=HAL_ERROR;

	if(IS_PYRO_SQUIB_CURRENT(current))
	{
		uint8_t pot_val=0;
		uint8_t pyro_squib_cnt=0;
		PyroSquibParam->current[PyroSquib]=current;
		
		//current to pot_val
		pot_val=DigPot_CurrentToPotVal(current);
		

		hal_err=DigPot_SetValue((uint8_t)PyroSquib, pot_val);
		if(hal_err!=HAL_OK)
		{
				return PYRO_SQUIB_I2C_ERR;
		}

	}
	else
	{
		return PYRO_SQUIB_INCORRECT_PARAM;	 
	}
	
	return PYRO_SQUIB_OK;	
}

enPyroSquibError PyroSquib_SetMask(uint8_t mask)
{
		PyroSquibParam->mask=mask;
}

enPyroSquibError PyroSquib_SetKeysState(enPyroSquibKeysState state)
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
				if(PyroSquibParam->mask&(1<<0)) HAL_GPIO_WritePin(PIR_EN1_GPIO_Port,PIR_EN1_Pin,GPIO_PIN_SET);
				if(PyroSquibParam->mask&(1<<1)) HAL_GPIO_WritePin(PIR_EN2_GPIO_Port,PIR_EN2_Pin,GPIO_PIN_SET);
				if(PyroSquibParam->mask&(1<<2)) HAL_GPIO_WritePin(PIR_EN3_GPIO_Port,PIR_EN3_Pin,GPIO_PIN_SET);
				if(PyroSquibParam->mask&(1<<3)) HAL_GPIO_WritePin(PIR_EN4_GPIO_Port,PIR_EN4_Pin,GPIO_PIN_SET);
				if(PyroSquibParam->mask&(1<<4)) HAL_GPIO_WritePin(PIR_EN5_GPIO_Port,PIR_EN5_Pin,GPIO_PIN_SET);
				if(PyroSquibParam->mask&(1<<5)) HAL_GPIO_WritePin(PIR_EN6_GPIO_Port,PIR_EN6_Pin,GPIO_PIN_SET);
				if(PyroSquibParam->mask&(1<<6)) HAL_GPIO_WritePin(PIR_EN7_GPIO_Port,PIR_EN7_Pin,GPIO_PIN_SET);
				if(PyroSquibParam->mask&(1<<7)) HAL_GPIO_WritePin(PIR_EN8_GPIO_Port,PIR_EN8_Pin,GPIO_PIN_SET);
		}
}

void PyroSquib_Timer_Start(void)
{
		pyro_pulse_counter=0;
		__HAL_TIM_SET_AUTORELOAD(&htim2, PyroSquibParam->period);
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,PyroSquibParam->pulse_time);
		__HAL_TIM_SET_COUNTER(&htim2, 0);
		__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC1);
		__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
		TIM_CCxChannelCmd(htim2.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
		__HAL_TIM_ENABLE(&htim2);
}


void PyroSquib_Timer_Stop(void)
{
		__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
		__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
		TIM_CCxChannelCmd(htim2.Instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);
		__HAL_TIM_DISABLE(&htim2);
}


#define PYRO_SQUIB_DELAY_START_MS	10
enPyroSquibError PyroSquib_Start(void)
{
	enPyroSquibNums PyroSquib=PYRO_SQUIB_1; 
	enPyroSquibError err=PYRO_SQUIB_OK;
	
//	//set current
//	for(PyroSquib=PYRO_SQUIB_1;PyroSquib<PYRO_SQUIB_4;PyroSquib++)
//	{
//			err=PyroSquib_SetCurrent(PyroSquib,PyroSquibParam->current[PyroSquib]);
//			if(err!=PYRO_SQUIB_OK)
//			{
//					return err;
//			}
//	}
	
	//enable current keys
	PyroSquib_SetKeysState(PYRO_SQUIB_KEYS_ON);
	PyroSquibParam->state=PYRO_SQUIB_RUN;
	
	
//	htim2.Instance->ARR=PyroSquibParam->pulse_time;
//	HAL_TIM_Base_Start_IT(&htim2);
	PyroSquib_Timer_Start();
	return PYRO_SQUIB_OK;
}

void PyroSquib_TimerExpired(void)
{
	pyro_pulse_counter++;
	if(pyro_pulse_counter<PyroSquibParam->pulses_num)
	{
	//disable current keys
		PyroSquib_SetKeysState(PYRO_SQUIB_KEYS_ON);	
	}
	else
	{
			PyroSquib_Timer_Stop();
			PyroSquibParam->state=PYRO_SQUIB_STOP;
	}
	
}

void PyroSquib_PulseEnd(void)
{
	//disable current keys
	PyroSquib_SetKeysState(PYRO_SQUIB_KEYS_OFF);	
}

