#include "adc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f1xx_hal.h"

#define ADC_TASK_STACK_SIZE	128
#define ADC_POLL_TIME				1


stADC_PyroBuf ADC_PyroBuf;


extern ADC_HandleTypeDef hadc1;

static void ADC_Task(void *pvParameters);
static float ADC_toVoltage(uint16_t adc_val);

volatile float ADC_voltage[ADC_CHN_NUM];
volatile  uint16_t ADC_value[ADC_CHN_NUM];

SemaphoreHandle_t xADCSemaphore;
extern DMA_HandleTypeDef hdma_adc1;

void ADC_Init(void)
{
		ADC_PyroBuf.buf_cnt=0;
		ADC_PyroBuf.fill_is_end=FALSE;
		ADC_PyroBuf.start_fill=FALSE;
	
		xADCSemaphore = xSemaphoreCreateBinary();
		xTaskCreate(ADC_Task,"ADC task",ADC_TASK_STACK_SIZE,NULL, tskIDLE_PRIORITY + 2, NULL);
}

static void ADC_Task(void *pvParameters)
{
		HAL_ADC_Start(&hadc1);
		uint8_t adc_cnt=0;
    for( ;; )
    {
			if (HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_value,ADC_CHN_NUM) == HAL_OK)
			{	
//					__HAL_DMA_DISABLE_IT(&hdma_adc1, DMA_IT_HT);  
//					__HAL_DMA_ENABLE_IT(&hdma_adc1, DMA_IT_TE);
					if( xSemaphoreTake( xADCSemaphore, ( TickType_t )100 ) == pdTRUE )
					{
							if(ADC_PyroBuf.start_fill)
							{
									for(adc_cnt=0;adc_cnt<ADC_CHN_NUM;adc_cnt++)
									{
										ADC_voltage[adc_cnt] = ADC_toVoltage(ADC_value[adc_cnt]);									
									  ADC_PyroBuf.buf[ADC_PyroBuf.buf_cnt+adc_cnt]=ADC_voltage[adc_cnt];
									}
									ADC_PyroBuf.buf_cnt+=ADC_CHN_NUM;
									
									if(ADC_PyroBuf.buf_cnt>=ADC_BUF_LEN)
									{
											ADC_PyroBuf.start_fill=0;
									}
							}
							else
							{
									for(adc_cnt=0;adc_cnt<ADC_CHN_NUM;adc_cnt++)
									{
										ADC_voltage[adc_cnt] = ADC_toVoltage(ADC_value[adc_cnt]);
									}
							}
					}	
					vTaskDelay(ADC_POLL_TIME);
			}	
    }
}

static float ADC_toVoltage(uint16_t adc_val)
{
		float voltage=(float)adc_val;
	
		return voltage;
}


void ADC_ConvComplete(void)
{
	static  BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;	
	xSemaphoreGiveFromISR( xADCSemaphore, &xHigherPriorityTaskWoken );
}

void ADC_FillBuf_Start(void)
{
		ADC_PyroBuf.buf_cnt=0;
		ADC_PyroBuf.start_fill=TRUE;
}

void ADC_FillBuf_Stop(void)
{
		ADC_PyroBuf.start_fill=FALSE;
}

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//	static  BaseType_t xHigherPriorityTaskWoken;
//	xHigherPriorityTaskWoken = pdFALSE;
//	if(hadc==&hadc1)
//	{
//			xSemaphoreGiveFromISR( xADCSemaphore, &xHigherPriorityTaskWoken );
//	}
//}
