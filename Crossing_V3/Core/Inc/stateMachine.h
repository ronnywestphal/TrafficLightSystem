

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

/* Define the delays */
typedef enum
{
	toggleFreq = 250,
	pedestrianDelay = 12000,
	walkingDelay = 10000,
	orangeDelay = 4000,
	greenDelay = 20000,
	redDelayMax = 15000

} delayEnum;

/* Define the states */
typedef enum
{
	Start,
	DefaultState,
	NoCarState,
	StayGreenState,
	YellowState,
	BlueState,
	BothBlueState,
	PedestrianGreenWait

} states;

#endif /* INC_STATEMACHINE_H_ */


#include "stdbool.h"
#include "stdint.h"

void stateCrossroad(void);






