#include "dig_pot.h"
#include "stm32f1xx_hal.h"
#include "main.h"


#define I2C_MUX_ADDR		0xE0
#define I2C_POT_ADDR		0x5C


uint8_t digPotValue[I2C_POT_NUM];

extern I2C_HandleTypeDef hi2c2;

void I2C_AFBusyBugWorkaround(I2C_HandleTypeDef* i2cHandle);
void I2C_DSBusyBugWorkaround(I2C_HandleTypeDef* i2cHandle);

void DigPot_Init(void)
{
		HAL_GPIO_WritePin(A0_AKT_GPIO_Port, A0_AKT_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(A1_AKT_GPIO_Port, A1_AKT_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(A2_AKT_GPIO_Port, A2_AKT_Pin, GPIO_PIN_RESET);

//		I2C_AFBusyBugWorkaround(&hi2c2);
		
}

HAL_StatusTypeDef DigPot_SetValue(enDigPot DigPot, uint8_t value)
{
	  uint8_t mux_reg;
		HAL_StatusTypeDef err=HAL_ERROR;
		
		digPotValue[DigPot]=value;
	
		mux_reg=(DigPot&0x3)|0x4;
	
		err=HAL_I2C_Master_Transmit(&hi2c2,I2C_MUX_ADDR,&mux_reg,1,10);
		if(err==HAL_BUSY)
		{
				return err;
		}
		else if(err==HAL_OK)
		{
				err=HAL_I2C_Master_Transmit(&hi2c2,I2C_POT_ADDR,&value,1,10);
		}
		
		return err;
}



//I2C busy bug

void I2C_AFBusyBugWorkaround(I2C_HandleTypeDef* i2cHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
//1. Disable the I2C peripheral by clearing the PE bit in I2Cx_CR1 register.
//		__HAL_I2C_DISABLE(i2cHandle);
		HAL_I2C_DeInit(i2cHandle);
    /**I2C1 GPIO Configuration    
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA 
    */
//2.  Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
    GPIO_InitStruct.Pin = SCL_AKT_Pin|SDA_AKT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SCL_SDA_AKT_GPIO_Port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(SCL_SDA_AKT_GPIO_Port, SCL_AKT_Pin|SDA_AKT_Pin, GPIO_PIN_SET);

//3.  Check SCL and SDA High level in GPIOx_IDR.
		while(HAL_GPIO_ReadPin(SCL_SDA_AKT_GPIO_Port, SCL_AKT_Pin)==GPIO_PIN_RESET);
		while(HAL_GPIO_ReadPin(SCL_SDA_AKT_GPIO_Port, SDA_AKT_Pin)==GPIO_PIN_RESET);
		
//4.  Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
//5.  Check SDA Low level in GPIOx_IDR.
		HAL_GPIO_WritePin(SCL_SDA_AKT_GPIO_Port, SDA_AKT_Pin, GPIO_PIN_RESET);
		while(HAL_GPIO_ReadPin(SCL_SDA_AKT_GPIO_Port, SDA_AKT_Pin)==GPIO_PIN_SET);

//6.  Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
//7.  Check SCL Low level in GPIOx_IDR.
		HAL_GPIO_WritePin(SCL_SDA_AKT_GPIO_Port, SCL_AKT_Pin, GPIO_PIN_RESET);
		while(HAL_GPIO_ReadPin(SCL_SDA_AKT_GPIO_Port, SCL_AKT_Pin)==GPIO_PIN_SET);

//8.  Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
//9.  Check SCL High level in GPIOx_IDR.
		HAL_GPIO_WritePin(SCL_SDA_AKT_GPIO_Port, SCL_AKT_Pin, GPIO_PIN_SET);
		while(HAL_GPIO_ReadPin(SCL_SDA_AKT_GPIO_Port, SCL_AKT_Pin)==GPIO_PIN_RESET);

//10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR).
//11. Check SDA High level in GPIOx_IDR.
		HAL_GPIO_WritePin(SCL_SDA_AKT_GPIO_Port, SDA_AKT_Pin, GPIO_PIN_SET);
		while(HAL_GPIO_ReadPin(SCL_SDA_AKT_GPIO_Port, SDA_AKT_Pin)==GPIO_PIN_RESET);
			
//12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
    GPIO_InitStruct.Pin = SCL_AKT_Pin|SDA_AKT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SCL_SDA_AKT_GPIO_Port, &GPIO_InitStruct);

//13. Set SWRST bit in I2Cx_CR1 register.
    SET_BIT(i2cHandle->Instance->CR1, I2C_CR1_SWRST);

//14. Clear SWRST bit in I2Cx_CR1 register.
    CLEAR_BIT(i2cHandle->Instance->CR1, I2C_CR1_SWRST);
//15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register.
//		__HAL_I2C_ENABLE(i2cHandle);
	HAL_I2C_Init(&hi2c2);
}
