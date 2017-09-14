#include "pyro_squib.h"
#include "stm32f1xx_hal.h"
#include "dig_pot.h"
#include "adc.h"
#include "cfg_info.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "string.h"

extern TIM_HandleTypeDef htim2;
extern sConfigInfo configInfo;	

uint8_t PyroSquibStatus;
uint8_t	pulse_time_expired=0;
uint16_t ADC_value_temp[ADC_CHN_NUM];
extern uint16_t ADC_value[ADC_CHN_NUM];
enPyroSquibError			PyroSquibError;  

SemaphoreHandle_t xPyroSquib_Semaphore=NULL;

stPyroSquib *PyroSquibParam=&configInfo.PyroSquibParams;

uint8_t  PyroSquib_Test(void);

#define PYRO_SQUIB_TASK_STACK_SIZE	128
static void PyroSquib_Task(void *pvParameters);

void 		 PyroSquib_Init(void)
{
		vSemaphoreCreateBinary( xPyroSquib_Semaphore );
		xTaskCreate(PyroSquib_Task,"Pyro squib task",PYRO_SQUIB_TASK_STACK_SIZE,NULL, tskIDLE_PRIORITY + 2, NULL);
}

enPyroSquibError PyroSquib_SetTime(uint16_t time)
{
	if(IS_PYRO_SQUIB_TIME(time))
	{
		PyroSquibParam->time=time;
	}
	else
	{
		return PYRO_SQUIB_INCORRECT_PARAM;
	}
	
	return PYRO_SQUIB_OK;
}

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
	
		switch(state)
		{
			case PYRO_SQUIB_KEYS_ON_MASK:
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
			break;
			
			case PYRO_SQUIB_KEYS_ON_ALL:
			{
					HAL_GPIO_WritePin(PIR_EN1_GPIO_Port,PIR_EN1_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(PIR_EN2_GPIO_Port,PIR_EN2_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(PIR_EN3_GPIO_Port,PIR_EN3_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(PIR_EN4_GPIO_Port,PIR_EN4_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(PIR_EN5_GPIO_Port,PIR_EN5_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(PIR_EN6_GPIO_Port,PIR_EN6_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(PIR_EN7_GPIO_Port,PIR_EN7_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(PIR_EN8_GPIO_Port,PIR_EN8_Pin,GPIO_PIN_SET);					
			}
			break;
			
			default:
			{
		
			}
			break;		
		}
}

#define PYRO_SQUIB_DELAY_START_MS	10
enPyroSquibError PyroSquib_Start(void)
{
	enPyroSquibNums PyroSquib=PYRO_SQUIB_1; 
	enPyroSquibError err=PYRO_SQUIB_OK;
	pulse_time_expired=0;
	//set current
	for(PyroSquib=PYRO_SQUIB_1;PyroSquib<PYRO_SQUIB_4;PyroSquib++)
	{
			err=PyroSquib_SetCurrent(PyroSquib,PyroSquibParam->current[PyroSquib]);
			if(err!=PYRO_SQUIB_OK)
			{
					return err;
			}
	}
	
	//enable current keys
	PyroSquib_SetKeysState(PYRO_SQUIB_KEYS_ON_MASK);
	PyroSquibParam->state=PYRO_SQUIB_RUN;
	
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_AUTORELOAD(&htim2, PyroSquibParam->time*10);
	HAL_TIM_Base_Start_IT(&htim2);
	
	while(pulse_time_expired==0)
	{
			taskYIELD();
	}
	
	return PYRO_SQUIB_OK;
}

void PyroSquib_TimerExpired(void)
{
	memcpy(ADC_value, ADC_value_temp, sizeof(uint16_t)*ADC_CHN_POT_NUM);
	PyroSquib_SetKeysState(PYRO_SQUIB_KEYS_OFF);	//disable current keys
	PyroSquibParam->state=PYRO_SQUIB_STOP;
	pulse_time_expired=1;
}


#define PYRO_SQUIB_WAIT_START_SEMAPHORE		1000
static void PyroSquib_Task(void *pvParameters)
{
		while(1)
		{
				if(xSemaphoreTake( xPyroSquib_Semaphore, PYRO_SQUIB_WAIT_START_SEMAPHORE ))
				{
						PyroSquibError=PyroSquib_Start();
				}
				else
				{
						PyroSquibStatus=PyroSquib_Test();
				}
		}
}


uint8_t  PyroSquib_Test(void)
{
		uint8_t i=0;
		uint8_t stat_temp=0;
		pulse_time_expired=0;
		PyroSquib_SetKeysState(PYRO_SQUIB_KEYS_ON_ALL);
		__HAL_TIM_SET_AUTORELOAD(&htim2, PYRO_SQUIB_TEST_TIME);
		HAL_TIM_Base_Start_IT(&htim2);	
		
		while(pulse_time_expired==0)
		{
				taskYIELD();
		}
		
		for(i=0;i<ADC_CHN_POT_NUM;i++)
		{
				if(ADC_value_temp[i]>PYRO_SQUIB_MIN_TEST_VOLTAGE)
				{
						stat_temp|=(1<<i);
				}
		}
		return stat_temp;
}


