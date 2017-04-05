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
	if(TRUE==xRxEnable)
	{
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	}
	else
	{
			__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
	}

	if(TRUE==xTxEnable)
	{
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
	}
	else
	{
			__HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
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


BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
		//HAL_UART_Transmit_IT(&huart1, (uint8_t *)&ucByte, 1); 
		huart1.Instance->DR=ucByte;
	  return TRUE;
}


BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
		//HAL_UART_Receive_IT(&huart1, (uint8_t *)pucByte, 1); 
		*pucByte=huart1.Instance->DR;
    return TRUE;
}


BOOL UART_IRQ_Handler(USART_TypeDef * usart) 
{
	if (usart == huart1.Instance) 
	{
		if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_RXNE) != RESET)) 
		{
			pxMBFrameCBByteReceived();
			//__HAL_UART_SEND_REQ(&huart1, UART_RXDATA_FLUSH_REQUEST);
			return TRUE;
		}
		if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) != RESET) &&(__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_TXE) != RESET)) 
		{
			pxMBFrameCBTransmitterEmpty();
			return TRUE;
		}
	}
	return FALSE;
}