/**
 ****************************************************************************
 * @brief   Test functions for the GPIO API, ISR and state machine functions
 * @file    test.c
 * @author  Ronny Westphal
 * @date    16-december-2022
 ****************************************************************************
 */

#include "test.h"

/* External typedefs and variables */
extern SPI_HandleTypeDef hspi3;
extern states State, NextState, PreviousState;
extern GpioPinPosition specificPin;
extern uint32_t msTimer, msTimer2;
extern bool carsVertical, carsHorizontal, greenVertical, NBtnPressed, SBtnPressed;
void testing(void){

	//testSetTrafficLights();
	//testGpioPinPosition();
	//testClearingAndPatterns();

	while (1){

		testStateChangesAndInterrupts();
	}
}

/* Test specific pins */
void testGpioPinPosition(void){
	GpioPinPosition pinPosition[] = {NE_Car,NW_Car,SE_Car,SW_Car,S_Ped,N_Ped,S_Blue,N_Blue};
	for (int i=0; i<8; i++){
		for (int j=0; j<3; j++){
			if (i<4){
				gpio_setTrafficLight(pinPosition[i], j);
			}
			else if (i<6 && j<2){
				gpio_setTrafficLight(pinPosition[i], j);
			} else if (i>5) {
				gpio_setTrafficLight(pinPosition[i], 0);
			}
			LED_pattern_transmit();
			HAL_Delay(200);
		}
	}
	HAL_Delay(500);
}

/* Test LED Patterns, clearing specific/multiple LEDs, toggle LEDS */
void testClearingAndPatterns(void){

	gpio_clearAllPins();
	gpio_setMultipleLights(verticalGreen);
	LED_pattern_transmit();
	HAL_Delay(800);

	gpio_clearAllPins();
	gpio_setMultipleLights(horizontalGreen);
	LED_pattern_transmit();
	HAL_Delay(800);

	for (int i=0; i<12; i++){
		gpio_togglePin(N_Blue); //tests state/set/clearPin
		if (i==6){
			gpio_clearAllNotBlue();
			gpio_setMultipleLights(yellowState);
		}
		LED_pattern_transmit();
		HAL_Delay(200);
	}
	gpio_togglePin(S_Blue);
	gpio_clearAllNotBlue();
	LED_pattern_transmit();
	HAL_Delay(500);
}

/**
 * @brief test interrupts/states. Change NextState and/or initialization to try from different states as starting point.
 *
 * BlueState: Blink for PedestrianDelay, green for walkingDelay
 * BothBlueState: Blink for pedestrianDelay, green and blink for walkingDelay, green for walkingDelay
 * YellowState: ends after yellowDelay but use switches to interrupt or change while-condition in state machine
 * DefaultState: change direction every redDelayMax
 * NoCarState: change direction every greenDelay
 * Start: Changes direction then goes to DefaultState or NoCarState
 * PedestrianGreenWait: Green for walkingDelay
 */
void testStateChangesAndInterrupts(void){
	while (1){
		// initialization
		gpio_clearAllPins();
		gpio_setMultipleLights(verticalGreen);
		greenVertical = true;
		LED_pattern_transmit();

		NextState = NoCarState;	    // <-- change to try different starting states

		while(NextState == NoCarState);	// NextState will change when interrupt initiates
		State = NextState;
		switch (State){
		case BothBlueState:
			NBtnPressed = true;		    //otherwise set by pressing button
			SBtnPressed = true;
			specificPin = S_Blue;   	//last pressed button
			break;
		case BlueState:
			NBtnPressed = true;
			specificPin = N_Blue;
			break;
		case YellowState:
			yellowLeds();
			break;
		case DefaultState:
			carsVertical = true;		//otherwise set by toggling switches
			carsHorizontal = true;
			break;
		case StayGreenState:
			carsVertical = true;		//greenVertical in initialization
			break;
		case Start: break;
		case PedestrianGreenWait: break;
		case NoCarState: break;
		}
		stateMachineCopy();
	}
}

/**
 * @brief state machine will run until it reaches the specified state in the while-condition.
 * 		  end of loop indicated by blue LEDs blinking
 */
void stateMachineCopy(void){
	uint32_t pDelay = pedestrianDelay;
	while(State != NoCarState){
		State = NextState;
		switch (State){
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
	gpio_clearAllPins();
	for (int i=0; i<8; i++){
		gpio_togglePin(N_Blue);
		gpio_togglePin(S_Blue);
		LED_pattern_transmit();
		HAL_Delay(100);
	}
}
