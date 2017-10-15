#include "mb.h"
#include "adc.h"
#include "string.h"
#include "main.h"
#include "dig_pot.h"
#include "pyro_squib.h"
#include "cfg_info.h"
#include "utilities.h"


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


extern stPyroSquib *PyroSquibParam;
extern uint16_t ADC_value[ADC_CHN_NUM];
extern enPyroSquibError			PyroSquibError;  
extern uint8_t pyroSquibStatus;
extern SemaphoreHandle_t xPyroSquib_Semaphore;

#define REG_INPUT_START     1000
#define REG_INPUT_NREGS     20

#define REG_HOLDING_START   2000
#define REG_HOLDING_NREGS   32

#define REG_ADC_0						0
#define REG_ADC_1						2
#define REG_ADC_2						4
#define REG_ADC_3						6
#define REG_ADC_4						8
#define REG_ADC_5						10
#define REG_ADC_6						12
#define REG_ADC_7						14


#define REG_PIR_STATE					16
#define REG_PIR_ERROR					17
#define REG_PIR_IN_LINE				18

static USHORT   usRegInputStart = REG_INPUT_START;
USHORT   usRegInputBuf[REG_INPUT_NREGS];

static USHORT   usRegHoldingStart = REG_HOLDING_START;
USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];


eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;
		float tempADCvalue;

    if ( ( usAddress >= REG_INPUT_START ) &&
         ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = (int) ( usAddress - usRegInputStart );
			
				tempADCvalue=ADC_toCurrent(ADC_value[0]);
				Float_To_UINT16_Buf(tempADCvalue, &usRegInputBuf[REG_ADC_0]);
				tempADCvalue=ADC_toCurrent(ADC_value[1]);
				Float_To_UINT16_Buf(tempADCvalue, &usRegInputBuf[REG_ADC_1]);
				tempADCvalue=ADC_toCurrent(ADC_value[2]);
				Float_To_UINT16_Buf(tempADCvalue, &usRegInputBuf[REG_ADC_2]);
				tempADCvalue=ADC_toCurrent(ADC_value[3]);
				Float_To_UINT16_Buf(tempADCvalue, &usRegInputBuf[REG_ADC_3]);
				tempADCvalue=ADC_toCurrent(ADC_value[4]);
				Float_To_UINT16_Buf(tempADCvalue, &usRegInputBuf[REG_ADC_4]);
				tempADCvalue=ADC_toCurrent(ADC_value[5]);
				Float_To_UINT16_Buf(tempADCvalue, &usRegInputBuf[REG_ADC_5]);
				tempADCvalue=ADC_toCurrent(ADC_value[6]);
				Float_To_UINT16_Buf(tempADCvalue, &usRegInputBuf[REG_ADC_6]);
				tempADCvalue=ADC_toCurrent(ADC_value[7]);
				Float_To_UINT16_Buf(tempADCvalue, &usRegInputBuf[REG_ADC_7]);			

						
				usRegInputBuf[REG_PIR_STATE]=PyroSquibParam->state;
				usRegInputBuf[REG_PIR_ERROR]=PyroSquibError;
				usRegInputBuf[REG_PIR_IN_LINE]=pyroSquibStatus;
			
			
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




#define REG_PIR_SET_TIME				0
#define REG_PIR_1_SET_CURRENT		1
#define REG_PIR_2_SET_CURRENT		3
#define REG_PIR_3_SET_CURRENT		5
#define REG_PIR_4_SET_CURRENT		7
#define REG_PIR_SET_MASK				9
#define REG_PIR_START						10

#define REG_PIR_1_CALIBR_CURRENT_K	11
#define REG_PIR_1_CALIBR_CURRENT_B	13
#define REG_PIR_2_CALIBR_CURRENT_K	15
#define REG_PIR_2_CALIBR_CURRENT_B	17
#define REG_PIR_3_CALIBR_CURRENT_K	19
#define REG_PIR_3_CALIBR_CURRENT_B	21
#define REG_PIR_4_CALIBR_CURRENT_K	23
#define REG_PIR_4_CALIBR_CURRENT_B	25

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
		uint8_t settingsNeedWrite=FALSE;
		float tempValue;
	
    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
        case MB_REG_READ:	
						usRegHoldingBuf[REG_PIR_SET_TIME]=PyroSquibParam->time;
				
						Float_To_UINT16_Buf(PyroSquibParam->current[0], &usRegHoldingBuf[REG_PIR_1_SET_CURRENT]);
						Float_To_UINT16_Buf(PyroSquibParam->current[1], &usRegHoldingBuf[REG_PIR_2_SET_CURRENT]);
						Float_To_UINT16_Buf(PyroSquibParam->current[2], &usRegHoldingBuf[REG_PIR_3_SET_CURRENT]);
						Float_To_UINT16_Buf(PyroSquibParam->current[3], &usRegHoldingBuf[REG_PIR_4_SET_CURRENT]);
				
						usRegHoldingBuf[REG_PIR_SET_MASK]=PyroSquibParam->mask;
						usRegHoldingBuf[REG_PIR_START]=0;
				
						Float_To_UINT16_Buf(PyroSquibParam->PyroSquibCurrentCalibr[0].k, &usRegHoldingBuf[REG_PIR_1_CALIBR_CURRENT_K]);
						Float_To_UINT16_Buf(PyroSquibParam->PyroSquibCurrentCalibr[0].b, &usRegHoldingBuf[REG_PIR_1_CALIBR_CURRENT_B]);
				
						Float_To_UINT16_Buf(PyroSquibParam->PyroSquibCurrentCalibr[1].k, &usRegHoldingBuf[REG_PIR_2_CALIBR_CURRENT_K]);
						Float_To_UINT16_Buf(PyroSquibParam->PyroSquibCurrentCalibr[1].b, &usRegHoldingBuf[REG_PIR_2_CALIBR_CURRENT_B]);
				
						Float_To_UINT16_Buf(PyroSquibParam->PyroSquibCurrentCalibr[2].k, &usRegHoldingBuf[REG_PIR_3_CALIBR_CURRENT_K]);
						Float_To_UINT16_Buf(PyroSquibParam->PyroSquibCurrentCalibr[2].b, &usRegHoldingBuf[REG_PIR_3_CALIBR_CURRENT_B]);
				
						Float_To_UINT16_Buf(PyroSquibParam->PyroSquibCurrentCalibr[3].k, &usRegHoldingBuf[REG_PIR_4_CALIBR_CURRENT_K]);
						Float_To_UINT16_Buf(PyroSquibParam->PyroSquibCurrentCalibr[3].b, &usRegHoldingBuf[REG_PIR_4_CALIBR_CURRENT_B]);				
				
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
											if(IS_PYRO_SQUIB_TIME(usRegHoldingBuf[REG_PIR_SET_TIME]) 	&&
													(PyroSquibParam->time!=usRegHoldingBuf[REG_PIR_SET_TIME]))
											{												
												PyroSquibParam->time=usRegHoldingBuf[REG_PIR_SET_TIME];
												settingsNeedWrite=TRUE;
											}
										}
										break;
										
										case REG_PIR_1_SET_CURRENT +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_1_SET_CURRENT], &tempValue);
											if(IS_PYRO_SQUIB_CURRENT(tempValue)	
												&& (FloatCheckEquality(PyroSquibParam->current[0],tempValue,FLOAT_EQ_EPSILON)==FALSE))
											{
												PyroSquibParam->current[0]=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;	
										
										case REG_PIR_2_SET_CURRENT +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_2_SET_CURRENT], &tempValue);
											if(IS_PYRO_SQUIB_CURRENT(tempValue)	
												&& (FloatCheckEquality(PyroSquibParam->current[1],tempValue,FLOAT_EQ_EPSILON)==FALSE))
											{
												PyroSquibParam->current[1]=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;	
										
										case REG_PIR_3_SET_CURRENT +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_3_SET_CURRENT], &tempValue);
											if(IS_PYRO_SQUIB_CURRENT(tempValue)	
												&& (FloatCheckEquality(PyroSquibParam->current[2],tempValue,FLOAT_EQ_EPSILON)==FALSE))
											{
												PyroSquibParam->current[2]=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;	
										
										case REG_PIR_4_SET_CURRENT +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_4_SET_CURRENT], &tempValue);
											if(IS_PYRO_SQUIB_CURRENT(tempValue)	
												&& (FloatCheckEquality(PyroSquibParam->current[3],tempValue,FLOAT_EQ_EPSILON)==FALSE))
											{
												PyroSquibParam->current[3]=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;											

										case REG_PIR_SET_MASK:
										{
												if(PyroSquibParam->mask!=usRegHoldingBuf[REG_PIR_SET_MASK])
												{
													PyroSquibParam->mask=usRegHoldingBuf[REG_PIR_SET_MASK];
													settingsNeedWrite=TRUE;
												}
										}
										break;	
										
										case REG_PIR_START:
										{
											 if(usRegHoldingBuf[REG_PIR_START])
											 {
													usRegHoldingBuf[REG_PIR_START]=0;
													xSemaphoreGive( xPyroSquib_Semaphore);
											 }
										}
										break;
										
										case REG_PIR_1_CALIBR_CURRENT_K +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_1_CALIBR_CURRENT_K], &tempValue);
											
											if(FloatCheckEquality(PyroSquibParam->PyroSquibCurrentCalibr[0].k, tempValue, FLOAT_EQ_EPSILON)==FALSE)
											{
												PyroSquibParam->PyroSquibCurrentCalibr[0].k=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;		

										case REG_PIR_1_CALIBR_CURRENT_B +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_1_CALIBR_CURRENT_B], &tempValue);
											if(FloatCheckEquality(PyroSquibParam->PyroSquibCurrentCalibr[0].b, tempValue, FLOAT_EQ_EPSILON)==FALSE)
											{
												PyroSquibParam->PyroSquibCurrentCalibr[0].b=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;	

										case REG_PIR_2_CALIBR_CURRENT_K +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_2_CALIBR_CURRENT_K], &tempValue);
											if(FloatCheckEquality(PyroSquibParam->PyroSquibCurrentCalibr[1].k, tempValue, FLOAT_EQ_EPSILON)==FALSE)
											{
												PyroSquibParam->PyroSquibCurrentCalibr[1].k=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;		

										case REG_PIR_2_CALIBR_CURRENT_B +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_2_CALIBR_CURRENT_B], &tempValue);
											if(FloatCheckEquality(PyroSquibParam->PyroSquibCurrentCalibr[1].b, tempValue, FLOAT_EQ_EPSILON)==FALSE)
											{
												PyroSquibParam->PyroSquibCurrentCalibr[1].b=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;

										case REG_PIR_3_CALIBR_CURRENT_K +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_3_CALIBR_CURRENT_K], &tempValue);
											if(FloatCheckEquality(PyroSquibParam->PyroSquibCurrentCalibr[2].k, tempValue, FLOAT_EQ_EPSILON)==FALSE)
											{
												PyroSquibParam->PyroSquibCurrentCalibr[2].k=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;		

										case REG_PIR_3_CALIBR_CURRENT_B +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_3_CALIBR_CURRENT_B], &tempValue);
											if(FloatCheckEquality(PyroSquibParam->PyroSquibCurrentCalibr[2].b, tempValue, FLOAT_EQ_EPSILON)==FALSE)
											{
												PyroSquibParam->PyroSquibCurrentCalibr[2].b=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;	

										case REG_PIR_4_CALIBR_CURRENT_K +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_4_CALIBR_CURRENT_K], &tempValue);
											if(FloatCheckEquality(PyroSquibParam->PyroSquibCurrentCalibr[3].k, tempValue, FLOAT_EQ_EPSILON)==FALSE)
											{
												PyroSquibParam->PyroSquibCurrentCalibr[3].k=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;		

										case REG_PIR_4_CALIBR_CURRENT_B +1:
										{
											UINT16_Buf_To_Float(&usRegHoldingBuf[REG_PIR_4_CALIBR_CURRENT_B], &tempValue);
											if(FloatCheckEquality(PyroSquibParam->PyroSquibCurrentCalibr[3].b, tempValue, FLOAT_EQ_EPSILON)==FALSE)
											{
												PyroSquibParam->PyroSquibCurrentCalibr[3].b=tempValue;
												settingsNeedWrite=TRUE;
											}
										}
										break;											

								}
								
								iRegIndex++;
                usNRegs--;
            }
						
						if(settingsNeedWrite)
						{
								ConfigInfoWrite();
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
