/**
 **************************************************************
 * @brief  State machine functions and ISR
 * @file   crossing_functions.c
 * @author Ronny Westphal
 * @date   16-december-2022
 * @brief  Functions to handle interrupts and state machine logic
 *****************************************************************
 */
/* Includes --------------------------------------------------*/
#include "crossing_functions.h"

/* External typedefs and variables */
extern states State, NextState, PreviousState;
extern GpioPinPosition specificPin;
extern uint32_t msTimer, msTimer2;
extern bool carsVertical, carsHorizontal, greenVertical, NBtnPressed, SBtnPressed;

/**
  * @brief ISR for buttons (pedestrians) and toggles (cars). It evaluates the system and makes the
  *        appropriate changes and decides the next state in the state machine.
  * @param GPIO_Pin: the pin that initiated the interrupt
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == N_Car_Pin || GPIO_Pin == S_Car_Pin || GPIO_Pin == E_Car_Pin || GPIO_Pin == W_Car_Pin){
		//save vertical and horizontal Cars new state
		carsVertical = HAL_GPIO_ReadPin(N_Car_GPIO_Port, N_Car_Pin)==0 || HAL_GPIO_ReadPin(S_Car_GPIO_Port, S_Car_Pin)==0;
		carsHorizontal = HAL_GPIO_ReadPin(E_Car_GPIO_Port, E_Car_Pin)==0 || HAL_GPIO_ReadPin(W_Car_GPIO_Port, W_Car_Pin)==0;

		if (State == Start || State == NoCarState || State == DefaultState || State == StayGreenState) {
			toggleInterruptNextState();
		}
	}

	if (GPIO_Pin == N_PSw_Pin && !NBtnPressed){
		if (specificPin != S_Blue){
			if (State != YellowState && greenVertical) {
				msTimer = msTimer2 = HAL_GetTick();
				NextState = BlueState;
			}
		} else NextState = BothBlueState;

		specificPin = N_Blue;
		NBtnPressed = true;
		if ((!greenVertical && !SBtnPressed) || (greenVertical && State == YellowState)){
			specificPin = 0x80;
			NBtnPressed = false;
		}
	}

	if (GPIO_Pin == S_PSw_Pin && !SBtnPressed){
		if (specificPin != N_Blue){
			if (State != YellowState && !greenVertical) {
				msTimer = msTimer2 = HAL_GetTick();
				NextState = BlueState;
			}
		} else NextState = BothBlueState;

		specificPin = S_Blue;
		SBtnPressed = true;
		if ((greenVertical && !NBtnPressed) || (!greenVertical && State == YellowState)){
			specificPin = 0x80;
			SBtnPressed = false;
		}
	}
}

/**
  * @brief Called from the ISR if a toggle is switched when the state machine
  * 		  is in either Start, DefaultState, NoCarState or StayGreenState
  */
void toggleInterruptNextState(){
	if (carsVertical && !carsHorizontal)
		NextState = greenVertical ? StayGreenState : YellowState;
	else if (!carsVertical && carsHorizontal)
		NextState = greenVertical ? YellowState : StayGreenState;
	else
		NextState = DefaultState;

	if (NextState == YellowState) yellowLeds(); //instant if no other cars (and not pedestrianGreenWait)
	if (NextState != DefaultState || PreviousState == StayGreenState) msTimer2 = HAL_GetTick(); //car arrives in toggleState...
																							 //...reset timer => wait redDelayMax
}

/**
  * @brief sets the cars LEDs to yellow and pedestrians LEDs to red
  */
void yellowLeds(){
	gpio_clearAllNotBlue();
	gpio_setMultipleLights(yellowState);
	LED_pattern_transmit();
	msTimer2 = HAL_GetTick();
}

/**
  * @brief called after a yellow light when the next state of the state machine needs to be evaluated
  */
void checkNextState(){
	if (State == BlueState) {
		afterBlueState();
	}
	else if (carsVertical && !carsHorizontal){
		gpio_clearAllNotBlue();
		gpio_setMultipleLights(verticalGreen);
		LED_pattern_transmit();
		greenVertical = true;
		NextState = StayGreenState;
	}
	else if (!carsVertical && carsHorizontal){
		gpio_clearAllNotBlue();
		gpio_setMultipleLights(horizontalGreen);
		LED_pattern_transmit();
		greenVertical = false;
		NextState = StayGreenState;
	}
	else {
		if (specificPin == 0x80)
			NextState = Start;
		else {
			NextState = BlueState;
			nextDirection();
			msTimer = msTimer2 = HAL_GetTick();
		}
	}
}

/**
  * @brief switches the direction of the LEDs
  */
void nextDirection(){
	gpio_clearAllPins();
	if (greenVertical){
		gpio_setMultipleLights(horizontalGreen);
	} else{
		gpio_setMultipleLights(verticalGreen);
	}
	greenVertical = greenVertical ? false : true;

	LED_pattern_transmit();
	msTimer = msTimer2 = HAL_GetTick();
}

/**
  * @brief toggles the blue LEDs after a pedestrian has pushed a button
  * 		  if another pedestrian pushes the other button then all blue LEDs will toggle
  */
void blueStateToggle(){
	if (HAL_GetTick() - msTimer2 > pedestrianDelay - orangeDelay){
		gpio_clearAllNotBlue();
		gpio_setMultipleLights(yellowState);
	}

	if (State == BothBlueState){
		gpio_togglePin(N_Blue);
		gpio_togglePin(S_Blue);
	} else gpio_togglePin(specificPin);

	LED_pattern_transmit();
	msTimer = HAL_GetTick();
}

/**
  * @brief called when all blue LEDs stops blinking
  */
void endBlueState(){
	clearPin(specificPin);
	checkNextState();
	msTimer = msTimer2 = HAL_GetTick();
	SBtnPressed = false;
	NBtnPressed = false;
	specificPin = 0x80;
}

/**
  * @brief decides the next state in the state machine after the blue LEDs stops blinking
  * 		  if there are cars in only one direction - that direction will receive and/or stay green
  */
void afterBlueState(){
	nextDirection();
	if (specificPin == N_Blue && !carsVertical && carsHorizontal)
		NextState = StayGreenState;
	else if (specificPin == S_Blue && carsVertical && !carsHorizontal)
		NextState = StayGreenState;
	else NextState = PedestrianGreenWait;
}
