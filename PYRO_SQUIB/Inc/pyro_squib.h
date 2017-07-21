#ifndef PYRO_SQUIB_H
#define PYRO_SQUIB_H

#include "stdint.h"
#include "dig_pot.h"


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

typedef struct
{
	uint16_t time; //on time in ms
	float current;
	uint8_t mask;
	enPyroSquibError error;
} stPyroSquib;

enPyroSquibError PyroSquib_SetParameters(stPyroSquib *PyroSquib, uint16_t time,float current, uint8_t mask);

enPyroSquibError PyroSquib_SetTime(stPyroSquib *PyroSquib, uint16_t time);
enPyroSquibError PyroSquib_SetCurrent(stPyroSquib *PyroSquib,float current);
enPyroSquibError PyroSquib_SetMask(stPyroSquib *PyroSquib, uint8_t mask);
enPyroSquibError PyroSquib_SetKeysState(stPyroSquib *PyroSquib, enPyroSquibKeysState state);
enPyroSquibError PyroSquib_Start(void);

void PyroSquib_TimerExpired(void);

#endif