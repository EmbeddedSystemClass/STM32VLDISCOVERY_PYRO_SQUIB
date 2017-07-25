#include "mb.h"
#include "adc.h"
#include "string.h"
#include "main.h"
#include "dig_pot.h"
#include "pyro_squib.h"
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
extern uint8_t digPotValue[I2C_POT_NUM];
extern stPyroSquib PyroSquibParam;
extern stADC_PyroBuf ADC_PyroBuf;
extern uint16_t ADC_value[ADC_CHN_NUM];

static enPyroSquibError			PyroSquibError;  

#define REG_INPUT_START     1001
#define REG_INPUT_NREGS     20

#define REG_HOLDING_START   2001
#define REG_HOLDING_NREGS   16

#define REG_ADC_0						0
#define REG_ADC_1						2
#define REG_ADC_2						4
#define REG_ADC_3						6
#define REG_ADC_4						8
#define REG_ADC_5						10
#define REG_ADC_6						12
#define REG_ADC_7						14


#define REG_PIR_STATE					16
#define REG_PIR_BUF_COUNTER		17
#define REG_PIR_FILL_IS_END		18
#define REG_PIR_ERROR					19

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
			
//				memcpy((void*)usRegInputBuf,(const void*)ADC_voltage,sizeof(float)*ADC_CHN_NUM);
			*(float*)&usRegInputBuf[REG_ADC_0]=ADC_value[0];
			*(float*)&usRegInputBuf[REG_ADC_1]=ADC_value[1];
			*(float*)&usRegInputBuf[REG_ADC_2]=ADC_value[2];
			*(float*)&usRegInputBuf[REG_ADC_3]=ADC_value[3];
			*(float*)&usRegInputBuf[REG_ADC_4]=ADC_value[4];
			*(float*)&usRegInputBuf[REG_ADC_5]=ADC_value[5];
			*(float*)&usRegInputBuf[REG_ADC_6]=ADC_value[6];
			*(float*)&usRegInputBuf[REG_ADC_7]=ADC_value[7];
			
				
				usRegInputBuf[REG_PIR_STATE]=PyroSquibParam.state;
				usRegInputBuf[REG_PIR_BUF_COUNTER]=ADC_PyroBuf.buf_cnt;
				usRegInputBuf[REG_PIR_FILL_IS_END]=ADC_PyroBuf.fill_is_end;
				usRegInputBuf[REG_PIR_ERROR]=PyroSquibError;
			
			
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




#define REG_PIR_SET_TIME			0
#define REG_PIR_SET_CURRENT		1
#define REG_PIR_SET_MASK			3
#define REG_PIR_START					4

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
						usRegHoldingBuf[REG_PIR_SET_TIME]=PyroSquibParam.time;
						usRegHoldingBuf[REG_PIR_SET_CURRENT]=PyroSquibParam.current;
						usRegHoldingBuf[REG_PIR_SET_MASK]=PyroSquibParam.mask;
						usRegHoldingBuf[REG_PIR_START]=0;
				
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

								
								switch(iRegIndex)
								{
										case REG_PIR_SET_TIME:
										{
												PyroSquibParam.time=usRegHoldingBuf[REG_PIR_SET_TIME];
										}
										break;
										
										case REG_PIR_SET_CURRENT:
										{
												PyroSquibParam.current=usRegHoldingBuf[REG_PIR_SET_CURRENT];
										}
										break;	

										case REG_PIR_SET_MASK:
										{
												PyroSquibParam.mask=usRegHoldingBuf[REG_PIR_SET_MASK];
										}
										break;	
										
										case REG_PIR_START:
										{
											 if(usRegHoldingBuf[REG_PIR_START])
											 {
													usRegHoldingBuf[REG_PIR_START]=0;
													PyroSquibError=PyroSquib_Start();
											 }
										}
										break;

								}
								
								iRegIndex++;
                usNRegs--;
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
