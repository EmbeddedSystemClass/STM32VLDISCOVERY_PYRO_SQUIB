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
	uint8_t pot_min;
	uint8_t pot_max;
	float 	current_min;
	float		current_max;
} stPyroSquibCurrentCalibr;

typedef struct
{
	//uint16_t time; //on time in ms
	uint16_t period;
	uint16_t pulse_time;
	uint16_t pulses_num;
	stPyroSquibCurrentCalibr calibr;
	float current[PYRO_SQUIB_NUM];
	uint8_t mask;
	enPyroSquibError error;
	enPyroSquibProcessState state;
} stPyroSquib;

#define PYRO_SQUIB_PERIOD_MIN		10
#define PYRO_SQUIB_PERIOD_MAX		500
#define IS_PYRO_SQUIB_PERIOD(__PERIOD__) (((__PERIOD__) >=PYRO_SQUIB_PERIOD_MIN) && ((__PERIOD__) <= PYRO_SQUIB_PERIOD_MAX))

#define PYRO_SQUIB_PULSE_TIME_MIN		10
#define PYRO_SQUIB_PULSE_TIME_MAX		500
#define IS_PYRO_SQUIB_PULSE_TIME(__PULSE_TIME__) (((__PULSE_TIME__) >=PYRO_SQUIB_PULSE_TIME_MIN) && ((__PULSE_TIME__) <= PYRO_SQUIB_PULSE_TIME_MAX) && ((__PULSE_TIME__) <= PyroSquibParam->period))

#define PYRO_SQUIB_PULSES_NUM_MIN		1
#define PYRO_SQUIB_PULSES_NUM_MAX		16
#define IS_PYRO_SQUIB_PULSES_NUM(__PULSES_NUM__) (((__PULSES_NUM__) >=PYRO_SQUIB_PULSES_NUM_MIN) && ((__PULSES_NUM__) <= PYRO_SQUIB_PULSES_NUM_MAX))

#define PYRO_SQUIB_CURRENT_MIN	0.5
#define PYRO_SQUIB_CURRENT_MAX	5.0
#define IS_PYRO_SQUIB_CURRENT(__CURRENT__) (((__CURRENT__) >=PYRO_SQUIB_CURRENT_MIN) && ((__CURRENT__) <= PYRO_SQUIB_CURRENT_MAX))



enPyroSquibError PyroSquib_SetTime(uint16_t time);
enPyroSquibError PyroSquib_SetCurrent(enPyroSquibNums PyroSquib,float current);
enPyroSquibError PyroSquib_SetMask(uint8_t mask);
enPyroSquibError PyroSquib_SetKeysState(enPyroSquibKeysState state);
enPyroSquibError PyroSquib_Start(void);

void PyroSquib_TimerExpired(void);
void PyroSquib_PulseEnd(void);

#endif