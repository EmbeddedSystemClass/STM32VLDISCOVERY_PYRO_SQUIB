#include "adc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f1xx_hal.h"

#define ADC_TASK_STACK_SIZE	128
#define ADC_POLL_TIME				1


extern ADC_HandleTypeDef hadc1;


volatile float ADC_voltage[ADC_CHN_NUM];
uint16_t ADC_value[ADC_CHN_NUM];

SemaphoreHandle_t xADCSemaphore;
extern DMA_HandleTypeDef hdma_adc1;

void ADC_Init(void)
{
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_value,ADC_CHN_NUM);
}
#define ADC_MAX_VAL			4095
#define ADC_MID_CONST		(ADC_MAX_VAL>>1)
#define ADC_REF_VOLTAGE	3.3
#define OPAMP_COEF			34.3333
float ADC_toVoltage(uint16_t adc_val)
{
		float voltage=((((float)adc_val/ADC_MAX_VAL)*ADC_REF_VOLTAGE)/OPAMP_COEF);
		return voltage;
}
