#include "adc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f1xx_hal.h"

#define ADC_TASK_STACK_SIZE	256
#define ADC_POLL_TIME				10
#define ADC_CHN_NUM					8

extern ADC_HandleTypeDef hadc1;

static void ADC_Task(void *pvParameters);
static float ADC_toVoltage(uint16_t adc_val);

float ADC_voltage[ADC_CHN_NUM];

void ADC_Init(void)
{
		xTaskCreate(ADC_Task,"ADC task",ADC_TASK_STACK_SIZE,NULL, tskIDLE_PRIORITY + 2, NULL);
}

static void ADC_Task(void *pvParameters)
{
		HAL_ADC_Start(&hadc1);
		uint8_t adc_cnt=0;
    for( ;; )
    {
			for(adc_cnt=0;adc_cnt<ADC_CHN_NUM;adc_cnt++)
			{
			  if (HAL_ADC_PollForConversion(&hadc1, 1000000) == HAL_OK)
        {
						ADC_voltage[adc_cnt] = ADC_toVoltage(HAL_ADC_GetValue(&hadc1));
				}
        vTaskDelay(ADC_POLL_TIME);
			}
    }
}

static float ADC_toVoltage(uint16_t adc_val)
{
		float voltage=0;
	
		return voltage;
}
