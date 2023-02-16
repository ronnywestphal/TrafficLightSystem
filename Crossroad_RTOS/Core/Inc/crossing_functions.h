#ifndef INC_CROSSING_FUNCTIONS_H
#define INC_CROSSING_FUNCTIONS_H

/* Define the states */
typedef enum
{
	Start,
	DefaultState,
	YellowState
} states;

/* Define the delays */
typedef enum
{
	toggleFreq = 250,		//ms
	pedestrianDelay = 1000,	//laps in MainTask: 1 = 10ms
	walkingDelay = 900,
	yellowDelay = 250,
	greenDelay = 2000,
	redDelayMax = 1500
} delayEnum;

#endif


#include "FreeRTOS.h"
#include "gpio_functions.h"
#include "stdbool.h"
#include "semphr.h"

void nextDirection(void);
void checkNextState(void);
void yellowLeds(void);
void blueStateToggle(void);
void nextBlueLedsAction(void);
void afterBlueState(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
