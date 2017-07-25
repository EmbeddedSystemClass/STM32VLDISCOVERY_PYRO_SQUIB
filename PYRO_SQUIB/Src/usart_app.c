/**
  ******************************************************************************
  * File Name          : USART_app.c
  * Date               : 31/07/2015 10:26:00
  * Description        : 
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "usart_app.h"

#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "port.h"


extern void prvvUARTTxReadyISR( void );
extern void prvvUARTRxISR( void );
extern volatile UCHAR ucRTUBuf[];
extern UART_HandleTypeDef huart1;


void startUARTRcv(UART_HandleTypeDef *huart)
{
// используем функцию приема по DMA, поэтому здесь просто запускаем прием по интерфейсам
	switch((uint32_t)huart->Instance)
	{
		case (uint32_t)USART1:	// Порт к ВВ работает по DMA!
		/*!!!!!! MB_SER_PDU_SIZE_MAX = 256 - mbrtu.c. Константу согласовывать! Bykov_DMA !!!!!!*/
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)&ucRTUBuf, 256);
    /* Enable the UART Data Register not empty Interrupt 
		Разрешим прерывания для контроля паузы между приемом байтов (конец фрейма) с помощью таймера */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
		break;
	}
}

/*
Останов приема и передачи при обмене с ВВ (остановить только прием при DMA проблематично)
Для полудуплекса вполне допустимо.
*/
void stopUART(UART_HandleTypeDef *huart)
{
	switch((uint32_t)huart->Instance)
	{
		case (uint32_t)USART1:
			HAL_UART_DMAStop(&huart1);
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
		break;
	}
	// остальные порты не останавливаются!
}

/*
Возвращаем количество принятых байтов по DMA по порту BB
*/
uint16_t getUART_BB_RcvDMAPktLength(void)
{
		return huart1.RxXferSize - huart1.hdmarx->Instance->CNDTR;
}

// Обработчик приема символов по СОМ портам (терминал и СпМ) в режиме прерываний
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) // from interrupt called!
{ 
//	int i, cnt;
//	uint8_t *pData;
	switch((uint32_t)UartHandle->Instance)
	{
		case (uint32_t)USART1: // При обмене с ВВ по DMA, попадем сюда после останова DMA по таймеру паузы
			// обработка буфера modbus идет в библиотеке и инициируется в обработчике таймера!
			
		// ДЛЯ ТЕСТА: определим сначала длину фрейма и передадим на обработку
/*		  cnt=getUART_BB_RcvDMAPktLength();
		  for(i=0, pData=UartHandle->pRxBuffPtr; i<cnt;i++,pData++){
				xQueueSendFromISR( myQueueUart6TxHandle , pData, NULL  );  
			}*/
		  break;
	}
}

// в случае ошибок просто взведем снова прием, т.к. иначе все остановится
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	startUARTRcv(huart);
}


// вызывается из прерывания после полного завершения передачи по СОМ порту
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	switch((uint32_t)huart->Instance)
	{
		case (uint32_t)USART1:	// modbus
			prvvUARTTxReadyISR();
		break;
	}

}
