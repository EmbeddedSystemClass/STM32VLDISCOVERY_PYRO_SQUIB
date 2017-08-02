#ifndef PYRO_SQUIB_H
#define PYRO_SQUIB_H

#include "stdint.h"
#include "dig_pot.h"

#define PYRO_SQUIB_NUM	4

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
	PYRO_SQUIB_KEYS_ON,
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
	uint16_t time; //on time in ms
	float current[PYRO_SQUIB_NUM];
	uint8_t mask;
	enPyroSquibError error;
	enPyroSquibProcessState state;
} stPyroSquib;

enPyroSquibError PyroSquib_SetTime(uint16_t time);
enPyroSquibError PyroSquib_SetCurrent(enPyroSquibNums PyroSquib,float current);
enPyroSquibError PyroSquib_SetMask(uint8_t mask);
enPyroSquibError PyroSquib_SetKeysState(enPyroSquibKeysState state);
enPyroSquibError PyroSquib_Start(void);

void PyroSquib_TimerExpired(void);

#endif