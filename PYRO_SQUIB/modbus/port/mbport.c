#include "mb.h"
#include "adc.h"
#include "string.h"
#include "main.h"
void ENTER_CRITICAL_SECTION(void)
{
	//__set_PRIMASK(1);
	__ASM volatile ("cpsid i");

}

void EXIT_CRITICAL_SECTION(void)
{
	//__set_PRIMASK(0);
	__ASM volatile ("cpsie i");
}

extern volatile float ADC_voltage[ADC_CHN_NUM];

#define REG_INPUT_START     1001
#define REG_INPUT_NREGS     16

#define REG_HOLDING_START   2001
#define REG_HOLDING_NREGS   8

#define REG_ADC_0						0
#define REG_ADC_1						2
#define REG_ADC_2						4
#define REG_ADC_3						6
#define REG_ADC_4						8
#define REG_ADC_5						10
#define REG_ADC_6						12
#define REG_ADC_7						14

static USHORT   usRegInputStart = REG_INPUT_START;
USHORT   usRegInputBuf[REG_INPUT_NREGS];

static USHORT   usRegHoldingStart = REG_HOLDING_START;
USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];


eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;

    if ( ( usAddress >= REG_INPUT_START ) &&
         ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = (int) ( usAddress - usRegInputStart );
			
				memcpy((void*)usRegInputBuf,(const void*)ADC_voltage,sizeof(float)*ADC_CHN_NUM);
        while ( usNRegs > 0 )
        {
            *pucRegBuffer++ = (unsigned char) ( usRegInputBuf[ iRegIndex ] >> 8 );
            *pucRegBuffer++ = (unsigned char) ( usRegInputBuf[ iRegIndex ] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

#define REG_PIR_EN1		0
#define REG_PIR_EN2		1
#define REG_PIR_EN3		2
#define REG_PIR_EN4		3
#define REG_PIR_EN5		4
#define REG_PIR_EN6		5
#define REG_PIR_EN7		6
#define REG_PIR_EN8		7

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
        case MB_REG_READ:
						usRegHoldingBuf[REG_PIR_EN1]=HAL_GPIO_ReadPin(PIR_EN1_GPIO_Port,PIR_EN1_Pin);
						usRegHoldingBuf[REG_PIR_EN2]=HAL_GPIO_ReadPin(PIR_EN2_GPIO_Port,PIR_EN2_Pin);
						usRegHoldingBuf[REG_PIR_EN3]=HAL_GPIO_ReadPin(PIR_EN3_GPIO_Port,PIR_EN3_Pin);
						usRegHoldingBuf[REG_PIR_EN4]=HAL_GPIO_ReadPin(PIR_EN4_GPIO_Port,PIR_EN4_Pin);
						usRegHoldingBuf[REG_PIR_EN5]=HAL_GPIO_ReadPin(PIR_EN5_GPIO_Port,PIR_EN5_Pin);
						usRegHoldingBuf[REG_PIR_EN6]=HAL_GPIO_ReadPin(PIR_EN6_GPIO_Port,PIR_EN6_Pin);
						usRegHoldingBuf[REG_PIR_EN7]=HAL_GPIO_ReadPin(PIR_EN7_GPIO_Port,PIR_EN7_Pin);
						usRegHoldingBuf[REG_PIR_EN8]=HAL_GPIO_ReadPin(PIR_EN8_GPIO_Port,PIR_EN8_Pin);
				
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;
        case MB_REG_WRITE:
					
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
								
								switch(iRegIndex)
								{
										case REG_PIR_EN1:
										{
							
											//	HAL_GPIO_WritePin(PIR_EN1_GPIO_Port,PIR_EN1_Pin,);
										}
										break;
										
										case REG_PIR_EN2:
										{
										}
										break;	

										case REG_PIR_EN3:
										{
										}
										break;	

										case REG_PIR_EN4:
										{
										}
										break;
										
										case REG_PIR_EN5:
										{
										}
										break;

										case REG_PIR_EN6:
										{
										}
										break;
										
										case REG_PIR_EN7:
										{
										}
										break;										
								}
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNCoils;
    ( void )eMode;
    return MB_ENOREG;
}


eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNDiscrete;
    return MB_ENOREG;
}
