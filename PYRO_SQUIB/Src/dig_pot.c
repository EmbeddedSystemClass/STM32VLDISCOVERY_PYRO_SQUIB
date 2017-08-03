#include "dig_pot.h"
#include "main.h"


#define I2C_MUX_ADDR		0xE0
#define I2C_POT_ADDR		0x5C

#define DIG_POT_MAX_CURRENT	5.0
#define DIG_POT_MAX_VALUE		127

uint8_t digPotValue[I2C_POT_NUM];

extern I2C_HandleTypeDef hi2c2;

void DigPot_Init(void)
{
		HAL_GPIO_WritePin(A0_AKT_GPIO_Port, A0_AKT_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(A1_AKT_GPIO_Port, A1_AKT_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(A2_AKT_GPIO_Port, A2_AKT_Pin, GPIO_PIN_RESET);
}

HAL_StatusTypeDef DigPot_SetValue(enDigPot DigPot, uint8_t value)
{
	  uint8_t mux_reg;
		HAL_StatusTypeDef err=HAL_ERROR;
		
		digPotValue[DigPot]=value;
	
		mux_reg=(DigPot&0x3)|0x4;
	
		err=HAL_I2C_Master_Transmit(&hi2c2,I2C_MUX_ADDR,&mux_reg,1,10);
		if(err==HAL_OK)
		{
					err=HAL_I2C_Master_Transmit(&hi2c2,I2C_POT_ADDR,&value,1,10);
		}
		
		return err;
}

uint8_t DigPot_CurrentToPotVal(float current)//0..127
{
		uint8_t pot_val=0;
		
		if((current<0)||(current>DIG_POT_MAX_CURRENT))
		{
				return 0;
		}
							
		pot_val= (uint8_t)((current/DIG_POT_MAX_CURRENT)*DIG_POT_MAX_VALUE);
	
		return pot_val; 
}