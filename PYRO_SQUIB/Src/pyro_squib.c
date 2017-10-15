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
#include "utilities.h"

extern TIM_HandleTypeDef htim2;
extern sConfigInfo configInfo;	
extern uint32_t SystemCoreClock; 

uint8_t 	pyroSquibStatus=0;
uint8_t		pulseTimeExpired=FALSE;
uint16_t 	ADC_value_temp[ADC_CHN_NUM]={0};
extern uint16_t ADC_value[ADC_CHN_NUM];
enPyroSquibError			PyroSquibError=PYRO_SQUIB_OK;  

SemaphoreHandle_t xPyroSquib_Semaphore=NULL;

stPyroSquib *PyroSquibParam=&configInfo.PyroSquibParams;

uint8_t  					PyroSquib_Test(void);
enPyroSquibError 	PyroSquib_SetCurrent(enPyroSquibNums PyroSquib,float current);
enPyroSquibError 	PyroSquib_SetCurrent_All(float current);
uint8_t 					PyroSquib_CurrentToPotVal(enPyroSquibNums PyroSquib, float current);

#define PYRO_SQUIB_TASK_STACK_SIZE	128
static void PyroSquib_Task(void *pvParameters);

void 		 PyroSquib_Init(void)
{
		xPyroSquib_Semaphore=xSemaphoreCreateBinary();
		__HAL_TIM_URS_ENABLE(&htim2);//для использования флага UG-обновления регистра прескалера
		PyroSquibError=PyroSquib_SetCurrent_All(PYRO_SQUIB_CURRENT_TEST);		
		xTaskCreate(PyroSquib_Task,"Pyro squib task",PYRO_SQUIB_TASK_STACK_SIZE,NULL, tskIDLE_PRIORITY + 2, NULL);
}


uint8_t PyroSquib_CurrentToPotVal(enPyroSquibNums PyroSquib, float current)//0..127
{
		uint8_t potVal=0;
		
		if(IS_PYRO_SQUIB_CURRENT(current))
		{
				potVal= (uint8_t)(PyroSquibParam->PyroSquibCurrentCalibr[PyroSquib].k*current + PyroSquibParam->PyroSquibCurrentCalibr[PyroSquib].b);
		}

		return potVal; 
}

enPyroSquibError PyroSquib_SetCurrent_All(float current)
{
		enPyroSquibNums PyroSquib=PYRO_SQUIB_1; 
		enPyroSquibError err=PYRO_SQUIB_OK, errTemp=PYRO_SQUIB_OK;
	
		for(PyroSquib=PYRO_SQUIB_1;PyroSquib<=PYRO_SQUIB_4;PyroSquib++)
		{
				errTemp=PyroSquib_SetCurrent(PyroSquib,current);
				if(errTemp!=PYRO_SQUIB_OK)
				{
						 err=errTemp;
				}
		}
		return err;
}

enPyroSquibError PyroSquib_SetCurrent(enPyroSquibNums PyroSquib, float current)
{
	HAL_StatusTypeDef hal_err=HAL_ERROR;

	if(IS_PYRO_SQUIB_CURRENT(current))
	{
		uint8_t potVal=0;
		
		potVal=PyroSquib_CurrentToPotVal(PyroSquib, current);		
		hal_err=DigPot_SetValue((uint8_t)PyroSquib, potVal);
		
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

enPyroSquibError PyroSquib_Start(void)
{
	enPyroSquibNums PyroSquib=PYRO_SQUIB_1; 
	enPyroSquibError err=PYRO_SQUIB_OK;
	pulseTimeExpired=FALSE;
	
	for(PyroSquib=PYRO_SQUIB_1;PyroSquib<=PYRO_SQUIB_4;PyroSquib++)//set current
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
	__HAL_TIM_SET_PRESCALER(&htim2,SystemCoreClock/1000);
	__HAL_TIM_SET_AUTORELOAD(&htim2, PyroSquibParam->time);
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
	htim2.Instance->EGR = TIM_EGR_UG;	
	HAL_TIM_Base_Start_IT(&htim2);

	
	while(pulseTimeExpired==FALSE)
	{
			//taskYIELD();
		vTaskDelay(50);
	}
	
	return PYRO_SQUIB_OK;
}

void PyroSquib_TimerExpired(void)
{
	memcpy(ADC_value_temp, ADC_value,sizeof(uint16_t)*ADC_CHN_POT_NUM);
	PyroSquib_SetKeysState(PYRO_SQUIB_KEYS_OFF);	//disable current keys
	PyroSquibParam->state=PYRO_SQUIB_STOP;
	pulseTimeExpired=TRUE;
}


#define PYRO_SQUIB_WAIT_START_SEMAPHORE		1000
static void PyroSquib_Task(void *pvParameters)
{
		while(1)
		{
				if(xSemaphoreTake( xPyroSquib_Semaphore, PYRO_SQUIB_WAIT_START_SEMAPHORE )== pdTRUE)
				{
						PyroSquibError=PyroSquib_Start();
						PyroSquibError=PyroSquib_SetCurrent_All(PYRO_SQUIB_CURRENT_TEST);
				}
				else
				{
						pyroSquibStatus=PyroSquib_Test();
				}
		}
}


uint8_t  PyroSquib_Test(void)
{
		uint8_t i=0;
		uint8_t pyroSquibInLine=0;
		pulseTimeExpired=FALSE;
		PyroSquib_SetKeysState(PYRO_SQUIB_KEYS_ON_ALL);
		__HAL_TIM_SET_AUTORELOAD(&htim2, PYRO_SQUIB_TEST_TIME);
		__HAL_TIM_SET_PRESCALER(&htim2,SystemCoreClock/10000);
		__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
		htim2.Instance->EGR = TIM_EGR_UG;
		HAL_TIM_Base_Start_IT(&htim2);	
		
		while(pulseTimeExpired==FALSE)
		{
				taskYIELD();
		}
		
		for(i=0;i<ADC_CHN_POT_NUM;i++)
		{
				if(ADC_toCurrent(ADC_value_temp[i])>PYRO_SQUIB_MIN_TEST_CURRENT)
				{
						pyroSquibInLine|=(1<<i);
				}
		}
		return pyroSquibInLine;
}


