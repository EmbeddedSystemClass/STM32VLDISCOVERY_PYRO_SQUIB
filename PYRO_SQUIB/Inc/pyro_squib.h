#ifndef PYRO_SQUIB_H
#define PYRO_SQUIB_H

#include "stdint.h"
#include "dig_pot.h"

#define PYRO_SQUIB_NUM	4

#define PYRO_SQUIB_TEST_TIME					1 //100uS
#define PYRO_SQUIB_MIN_TEST_VOLTAGE		300 //0-4095
#define PYRO_SQUIB_MIN_TEST_CURRENT		0.3 //A

typedef enum
{
	PYRO_SQUIB_1=0,
	PYRO_SQUIB_2,
	PYRO_SQUIB_3,
	PYRO_SQUIB_4,	
}enPyroSquibNums;

typedef enum
{
	PYRO_SQUIB_OK=0,
	PYRO_SQUIB_I2C_ERR,
	PYRO_SQUIB_INCORRECT_PARAM,
}
enPyroSquibError;

typedef enum
{
	PYRO_SQUIB_KEYS_OFF=0,
	PYRO_SQUIB_KEYS_ON_MASK,
	PYRO_SQUIB_KEYS_ON_ALL,
}
enPyroSquibKeysState;

typedef enum
{
  PYRO_SQUIB_STOP=0,
	PYRO_SQUIB_RUN,
}
enPyroSquibProcessState;

typedef struct
{
	float k;
	float b;
}stPyroSquibCurrentCalibr;

typedef struct
{
	uint16_t time; //on time in ms
	float current[PYRO_SQUIB_NUM];
	uint8_t mask;
	stPyroSquibCurrentCalibr PyroSquibCurrentCalibr[PYRO_SQUIB_NUM];
	enPyroSquibError error;
	enPyroSquibProcessState state;
} stPyroSquib;


#define PYRO_SQUIB_TIME_MIN		10		//10ms
#define PYRO_SQUIB_TIME_MAX		10000 //10 s
#define IS_PYRO_SQUIB_TIME(__TIME__) (((__TIME__) >=PYRO_SQUIB_TIME_MIN) && ((__TIME__) <= PYRO_SQUIB_TIME_MAX))

#define PYRO_SQUIB_CURRENT_MIN	0
#define PYRO_SQUIB_CURRENT_MAX	127
#define PYRO_SQUIB_CURRENT_TEST	1.0
#define IS_PYRO_SQUIB_CURRENT(__CURRENT__) (((__CURRENT__) >=PYRO_SQUIB_CURRENT_MIN) && ((__CURRENT__) <= PYRO_SQUIB_CURRENT_MAX))

#define PYRO_SQUIB_CURRENT_CALIBR_K_DEFAULT	(43.541)
#define PYRO_SQUIB_CURRENT_CALIBR_B_DEFAULT	(-0.043541)


void 		 					PyroSquib_Init(void);
enPyroSquibError 	PyroSquib_SetKeysState(enPyroSquibKeysState state);
enPyroSquibError 	PyroSquib_Start(void);
void 							PyroSquib_TimerExpired(void);

#endif