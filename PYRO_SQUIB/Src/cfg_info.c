/*
Модуль работы с переменными и константами
*/

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "cfg_info.h"
#include "pyro_squib.h"

#include "FreeRTOS.h"
#include "task.h"

//****************************************************************************
// именованные константы

//****************************************************************************
const sConfigInfo configInfoHard = {
	LABEL_CFG_SECTOR,	// Поле не убирать! Метка активного сектора!
	{	PYRO_SQUIB_TIME_DEFAULT,
		{PYRO_SQUIB_CURRENT_DEFAULT,PYRO_SQUIB_CURRENT_DEFAULT,PYRO_SQUIB_CURRENT_DEFAULT,PYRO_SQUIB_CURRENT_DEFAULT},
		PYRO_SQUIB_MASK_DEFAULT,
		PYRO_SQUIB_OK,
		PYRO_SQUIB_STOP
	}
};

//****************************************************************************
// глобальные переменные доступные извне
sConfigInfo configInfo;			// структура для хранения конфигурации прибора в ОЗУ

//****************************************************************************
// внутренние переменные

// указатели на хранимые во FLASH буферы конфигурационной информации
// одновременно активен только один буфер
static sConfigInfo *pMyInfoActive;

// для хранения и возможности переписывания информации используется 2 банка данной информации
// работающие по очереди.
static sConfigInfo MyInfo __attribute__((section(".flash_cfg1")));
static sConfigInfo MyInfo1 __attribute__((section(".flash_cfg2")));

void Flash_Write_Task( void *pvParameters );

//****************************************************************************

static int FLASH_ErasePage (uint32_t * pageAddr)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;


	// * Erase the Flash area * /
	// * Fill EraseInit structure* /

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks = 0;
  EraseInitStruct.NbPages =1;
	
  if((uint32_t) pageAddr==(uint32_t) &MyInfo)
	{
		EraseInitStruct.PageAddress = FLASH_PAGE_1;
	}
	else
	{
		EraseInitStruct.PageAddress = FLASH_PAGE_2;
	}
		
		// * Unlock the Flash to enable the flash control register access ************* /
	HAL_FLASH_Unlock();
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK){					
		HAL_FLASH_Lock(); 
		return -1;
	}
	HAL_FLASH_Lock(); 
	return 0;
}

// процедура записи во FLASH блока данных 
// return value: 0 - OK, -1 - ERROR

static int FlashWrite_32(uint32_t * from, uint32_t * to, uint16_t cnt)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;

	// * Unlock the Flash to enable the flash control register access ************* / 

	// * Erase the Flash area * /
	// * Fill EraseInit structure* /
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks = 0;
  EraseInitStruct.NbPages =1;
	
  if((uint32_t) to==(uint32_t) &MyInfo)
	{
		EraseInitStruct.PageAddress = FLASH_PAGE_1;
	}
	else
	{
		EraseInitStruct.PageAddress = FLASH_PAGE_2;
	}
	
	HAL_FLASH_Unlock();
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK){					
		HAL_FLASH_Lock(); 
		return -1;
	}
	for (int i=0; i<cnt; i++, from++, to++)	{ // запишем на 1 слово больше, чтобы исключить "обрезание" при делении на 4
		if (HAL_FLASH_Program(TYPEPROGRAM_WORD, (uint32_t)to, *from) != HAL_OK)	{
			HAL_FLASH_Lock();
			return -1;
		}
	}
	HAL_FLASH_Lock();
	return 0;
}

//****************************************************************************
// процедура считывания конфигурационной информации прибора

int ConfigInfoRead (void)
{
// определим активный буфер конфигурационной информации
  if(strcmp(MyInfo.Label, LABEL_CFG_SECTOR) == 0){			//
    pMyInfoActive = &MyInfo;			// активный буфер - 1
//	printf("MyInfo\r\n");
  }
  else{
    if(strcmp(MyInfo1.Label, LABEL_CFG_SECTOR) == 0){	//
      pMyInfoActive = &MyInfo1;		// активный буфер - 2
//	  printf("MyInfo1\r\n");
    }
    else{	// странно, но ни в одном буфере нет нужной информации - восстановим ее! (либо первый запуск прибора)
      if(FlashWrite_32((uint32_t*)&configInfoHard, (uint32_t*)&MyInfo, sizeof(MyInfo)/4 +1 ) < 0){
//				memcpy(&configInfo, &configInfoHard, sizeof(MyInfo));
        return -1;
      }
      // сотрем второй буфер FLASH
      if(FLASH_ErasePage((uint32_t*)&MyInfo1) < 0){
        return -1;
      }
      pMyInfoActive = &MyInfo;			// активный буфер - 1
//	  printf("configInfoHard\r\n");
    }
  }
  memcpy(&configInfo, pMyInfoActive, sizeof(MyInfo));	
  return 0;
}

//****************************************************************************
// Запись новой информации о приборе (в виде структуры) во FLASH из configInfo
// return 0 - OK, -1 - Error

int ConfigInfoWrite(void)
{
	sConfigInfo *pMyInfoNoActive;
  if(pMyInfoActive == &MyInfo){
		pMyInfoNoActive = &MyInfo1;
	}
	else{
		pMyInfoNoActive = &MyInfo;
	}

	if(FlashWrite_32((uint32_t*)&configInfo, (uint32_t*)pMyInfoNoActive, sizeof(MyInfo)/4 +1 ) < 0){ // запишем обновление во FLASH
    return -1;
  }
  // сотрем второй буфер FLASH
  if(FLASH_ErasePage((uint32_t*)pMyInfoActive) < 0){
    return -1;
  }
  pMyInfoActive = pMyInfoNoActive;
  return 0;
}

void StartConfigInfoWrite(void)
{
	xTaskCreate( Flash_Write_Task, "Flash Write Task", 128, NULL, 4, NULL );
}

void Flash_Write_Task( void *pvParameters )
{
	ConfigInfoWrite();
	vTaskDelete(NULL);
}
