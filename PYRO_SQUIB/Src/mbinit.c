#include "mbinit.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "mb.h"
#include "mbport.h"


#include "stdio.h"
#include "errno.h"



#define MODBUS_DEV_ADDR	0x0A


#define MODBUS_TASK_STACK_SIZE	128
#define MODBUS_BAUDRATE			921600
#define MODBUS_POLL_TIME		10


static void Modbus_Task(void *pvParameters);


void Modbus_Init(void)
{
	eMBErrorCode    eStatus;

	eStatus = eMBInit(MB_RTU, MODBUS_DEV_ADDR, 0, MODBUS_BAUDRATE, 0 );

	xTaskCreate(Modbus_Task,"Modbus task",MODBUS_TASK_STACK_SIZE,NULL, tskIDLE_PRIORITY + 1, NULL);

}



static void Modbus_Task(void *pvParameters)
{
    eMBEnable();

    for( ;; )
    {
        eMBPoll();
    }
}

