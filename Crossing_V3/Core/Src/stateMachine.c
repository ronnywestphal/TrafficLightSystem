/**
 *************************************************************************
 * @brief State machine for Traffic Lights Project
 * @file  stateMachine.c
 * @author Ronny Westphal
 * @version 1.0
 * @date    16-december-2022
 *************************************************************************
 */
/* Includes -------------------------------------------------------------*/
#include "crossing_functions.h"
#include "stateMachine.h"
#include "gpio_functions.h"

/* Enums for states and a specific pin  */
states State, NextState, PreviousState;
GpioPinPosition specificPin = 0x80;       // 0x80 is not used in the shift register

/* Timers for the delays */
uint32_t msTimer = 0, msTimer2 = 0;

/* booleans for switches/buttons and current traffic light direction */
bool carsVertical = false;
bool carsHorizontal = false;
bool NBtnPressed = false;
bool SBtnPressed = false;
bool greenVertical = false;

/**
 * @brief state machine for the traffic lights
 */
void stateCrossroad(void){
	NextState = Start;
	uint32_t pDelay = pedestrianDelay;
	while (1)
	{
		PreviousState = State;
		State = NextState;

		switch(State)
		{
		case Start:
			nextDirection();
			NextState = carsVertical || carsHorizontal ? DefaultState : NoCarState;  //switching green states
			break;

		case DefaultState: //default switching green with cars
			if (HAL_GetTick() - msTimer2 > redDelayMax - orangeDelay){
				yellowLeds();
				NextState = YellowState;
			}
			break;

		case YellowState:
			if (HAL_GetTick() - msTimer2 > orangeDelay)
				checkNextState();
			break;

		case NoCarState: //switching green no cars
			if (HAL_GetTick() - msTimer2 > greenDelay - orangeDelay){
				yellowLeds();
				NextState = YellowState;
			}
			break;

		case PedestrianGreenWait: //green after button press: prevents instant green if car arrives in NoCarState
			if (HAL_GetTick() - msTimer2 > walkingDelay - orangeDelay){
				yellowLeds();
				NextState = YellowState;
			}
			break;

		case StayGreenState:
			break;

		case BlueState:
			pDelay = PreviousState == BothBlueState ? walkingDelay : pedestrianDelay;
			if (HAL_GetTick() - msTimer2 > pDelay)
				endBlueState();
			else if (HAL_GetTick() - msTimer > toggleFreq)
				blueStateToggle();
			break;

		case BothBlueState:
			if (HAL_GetTick() - msTimer2 > pedestrianDelay) {
				if (specificPin == N_Blue){
					SBtnPressed = false;
				} else NBtnPressed = false;

				nextDirection();
				NextState = BlueState;
			} else if (HAL_GetTick() - msTimer > toggleFreq){
				blueStateToggle();
			}
			break;
		}
	}
}

