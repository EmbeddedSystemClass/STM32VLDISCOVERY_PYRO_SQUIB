#include "mb.h"
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



#define REG_INPUT_START     1001
#define REG_INPUT_NREGS     15

#define REG_HOLDING_START   2001
#define REG_HOLDING_NREGS   4

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
