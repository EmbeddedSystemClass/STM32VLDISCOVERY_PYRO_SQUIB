#ifndef ADC_H
#define ADC_H

#include "stdint.h"

#define ADC_CHN_NUM					8

#define ADC_NUM_SAMPLES			50

#define ADC_BUF_LEN					(ADC_CHN_NUM*ADC_NUM_SAMPLES)

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif


typedef struct 
{
	float buf[ADC_BUF_LEN];
	uint16_t buf_cnt;
	uint8_t  fill_is_end;
	uint8_t  start_fill;	
}
stADC_PyroBuf;


void ADC_Init(void);
void ADC_ConvComplete(void);
void ADC_FillBuf_Start(void);
void ADC_FillBuf_Stop(void);
float ADC_toVoltage(uint16_t adc_val);
#endif