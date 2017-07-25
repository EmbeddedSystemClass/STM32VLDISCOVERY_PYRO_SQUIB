/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "mbport.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;
/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable ) 
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	// Нам нужно запустить прием и прерывания по RX, если xRxEnable, иначе запретить.
	if(xRxEnable){
		startUARTRcv(&huart1);
	}
	else{
		stopUART(&huart1);
	}
	// Нам нужно запустить передачу и прерывания по TX, если xTxEnable, иначе запретить.
	// !!! Причем стек сам не запустит прием, если он не примет событие "буфер передатчика свободен"
	// В нашем случае прямо вызову pxMBFrameCBTransmitterEmpty(  );
	//!!! Не сделан запрет!!!
	if(xTxEnable){
		pxMBFrameCBTransmitterEmpty(  );
	}
}

BOOL 
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
  huart1.Instance = USART1;	
  huart1.Init.BaudRate = ulBaudRate;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = eParity;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);
  return TRUE;
}


//not used for DMA!
//static CHAR txByte;	// отправляемый байт должен быть доступен во время отправки
//not used for DMA!
BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
/*		UART_BB_DIR_SEND();
		txByte = ucByte;	// мы только заряжаем отправку. Во время отправки буфер должен быть доступен, поэтому используем глобальную переменную
		HAL_UART_Transmit_IT(&H_UART_BB, (uint8_t *)&txByte, 1); // заряжаем...
*/
    return TRUE;
}

/*
Инициализация отправки по DMA
*/
BOOL
xMBPortSerialPutPktDMA( CHAR *pucSndBuffer, USHORT usSndBufferCount)
{
		HAL_UART_Transmit_DMA(&huart1, (uint8_t*)pucSndBuffer, usSndBufferCount);
		return TRUE;
}

//not used for DMA!
BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
//		*pucByte = CH_UART_BB;
		*pucByte = 0;
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
//static unsigned int uiCnt = 0;
/*
При использовании DMA вызывается в конце выдачи единожды - нужно сразу вызвать необходимую функцию
*/
void prvvUARTTxReadyISR( void )
{
    BOOL bTaskWoken = FALSE;
	
//		vMBPortSetWithinException( TRUE );
/*    if( uiCnt++ < 10 )
    {
        ( void )xMBPortSerialPutByte( 'a' );
    }
    else
    {
        vMBPortSerialEnable( FALSE, FALSE );
    }*/
		bTaskWoken = pxMBFrameCBTransmitterEmpty(  );
//    vMBPortSetWithinException( FALSE );

    portEND_SWITCHING_ISR( bTaskWoken ? pdTRUE : pdFALSE );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
//not used for DMA!
/*void prvvUARTRxISR( void )
{
    BOOL bTaskWoken = FALSE;

		vMBPortSetWithinException( TRUE );
		
		bTaskWoken = pxMBFrameCBByteReceived(  );
    
		vMBPortSetWithinException( FALSE );

    portEND_SWITCHING_ISR( bTaskWoken ? pdTRUE : pdFALSE );
}
*/
