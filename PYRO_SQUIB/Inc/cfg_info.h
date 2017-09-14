/*
Модуль хранения энергонезависимой конфигурации во FLASH. 
Обеспечена отказоустойчивость к отключению питания во время обновления (транзакция записи)
Используется 2 сектора FLASH, только один из которых активен.
При обновлении данных сначала производится запись в неактивный сектор, после чего стирается активный и назначается новый активный
Признаком активности служит метка в начале сектора.
Структура хранимой информации описана структурой sConfigInfo
Для инициализации при первом старте или каком либо сбое используются значения по умолчанию, хранимые в "const sConfigInfo configInfoHard",
инициализацию которой рекомендуется выполнять в головном модуле (main.c, например)

ВАЖНО!
При использовании модуля должен использоваться sct файл с выделенными 2-мя секторами для хранения информации. 
Например:
LR_IROM2 0x08000000 0x00004000  {    ; load region size_region
  FLASH_CFG1 0x08000000 0x00004000  {  ; load address = execution address
   *.o (.flash_cfg1)
  }
}
LR_IROM3 0x08004000 0x00004000  {    ; load region size_region
  FLASH_CFG2 0x08004000 0x00004000  {  ; load address = execution address
   *.o (.flash_cfg2)
  }
}

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!Модуль целесообразно использовать в случае нечастого изменения конфигурационной информации (в связи с ограничением числа циклов перезаписи FLASH)!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
#ifndef _INCL_CFG_INFO_H
#define _INCL_CFG_INFO_H

#include "main.h"
#include "stm32f1xx_hal.h"
#include "pyro_squib.h"


#define FLASH_PAGE_1		(FLASH_BASE + FLASH_PAGE_SIZE)
#define FLASH_PAGE_2		(FLASH_BASE + FLASH_PAGE_SIZE + FLASH_PAGE_SIZE)

#define LABEL_CFG_SECTOR	"CFG_SECTOR"
#define PYRO_SQUIB_TIME_DEFAULT			100
#define PYRO_SQUIB_CURRENT_DEFAULT	1
#define PYRO_SQUIB_MASK_DEFAULT	0xFF




// конфигурационная информация (EEPROM)
typedef struct 
{
	char Label[16];           
	stPyroSquib PyroSquibParams; 
} sConfigInfo;

extern sConfigInfo configInfo;			// структура для хранения конфигурации прибора в ОЗУ
// процедура считывания конфигурационной информации прибора
int ConfigInfoRead (void);
//****************************************************************************
// Обновление информации о приборе (в виде структуры) во FLASH из MyInfoVar
int ConfigInfoWrite(void);
void StartConfigInfoWrite(void);

#endif

