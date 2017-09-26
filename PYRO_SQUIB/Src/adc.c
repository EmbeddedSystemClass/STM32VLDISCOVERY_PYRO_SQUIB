#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f1xx_hal.h"

extern ADC_HandleTypeDef hadc1;
uint16_t ADC_value[ADC_CHN_NUM];


extern DMA_HandleTypeDef hdma_adc1;

void ADC_Init(void)
{
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_value,ADC_CHN_NUM);
}



float ADC_toCurrent(uint16_t adc_val)
{
		float current= (((float)adc_val-ADC_CALIBR_MIN_VAL)*(ADC_CALIBR_MAX_CUR-ADC_CALIBR_MIN_CUR)/(ADC_CALIBR_MAX_VAL-ADC_CALIBR_MIN_VAL)+ADC_CALIBR_MIN_CUR);
		return current;
}
