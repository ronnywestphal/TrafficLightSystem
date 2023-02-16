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
states State, NextState;
extern GpioPinPosition specificPin;
extern bool carsVertical, carsHorizontal, greenVertical, NBtnPressed, SBtnPressed, stayGreen;
extern uint32_t waitTime;
extern SemaphoreHandle_t NbinarySem;

/**
  * @brief switches the direction of the LEDs
  */
void nextDirection(void){
	gpio_clearAllNotBlue();
	if (greenVertical){
		gpio_setMultipleLights(horizontalGreen);
	} else{
		gpio_setMultipleLights(verticalGreen);
	}
	greenVertical = greenVertical ? false : true;
}

/**
  * @brief called after a yellow light when the next state of the state machine needs to be evaluated
  */
void checkNextState(void){
	if (NBtnPressed || SBtnPressed){
		waitTime = walkingDelay;
		stayGreen = false;
	} else {
		if (!carsVertical && !carsHorizontal) {
			waitTime = carsVertical ? redDelayMax : greenDelay;
		}
		else if (carsVertical != carsHorizontal){
			gpio_clearAllNotBlue();
			if (carsVertical){
				gpio_setMultipleLights(verticalGreen);
				greenVertical = true;
			} else {
				gpio_setMultipleLights(horizontalGreen);
				greenVertical = false;
			}
		}
		stayGreen = carsVertical != carsHorizontal ? true : false;
	}
	NextState = Start;
}

/**
  * @brief sets the cars LEDs to yellow and pedestrians LEDs to red
  */
void yellowLeds(void){
	gpio_clearAllNotBlue();
	gpio_setMultipleLights(yellowState);
}

/**
  * @brief toggles the blue LEDs after a pedestrian has pushed a button
  * 	   if another pedestrian pushes the other button then all blue LEDs will toggle
  */
void blueStateToggle(void){
	if (NBtnPressed)
		gpio_togglePin(N_Blue);
	if (SBtnPressed)
		gpio_togglePin(S_Blue);
}

void nextBlueLedsAction(void){
	if (greenVertical){
		NBtnPressed = false;
		clearPin(N_Blue);
	}
	else {
		SBtnPressed = false;
		clearPin(S_Blue);
	}
	if (!NBtnPressed && !SBtnPressed && waitTime != pedestrianDelay) {
		xSemaphoreTake(NbinarySem,(TickType_t)0);
		if (specificPin != 0x80) afterBlueState();
	}
}

/**
  * @brief decides what to do after the blue LEDs stops blinking
  * 		  if there are cars in only one direction - that direction will become and/or stay green
  */
void afterBlueState(void){
	if (specificPin == N_Blue && !carsVertical && carsHorizontal){
		nextDirection();
		waitTime = redDelayMax;
		stayGreen = true;
	}
	else if (specificPin == S_Blue && carsVertical && !carsHorizontal){
		nextDirection();
		waitTime = redDelayMax;
		stayGreen = true;
	}
	else {
		waitTime = walkingDelay;
		stayGreen = false;
	}
	NextState = Start;
	specificPin = 0x80;

}

/**
  * @brief ISR for buttons (pedestrians) and toggles (cars). It evaluates the system and makes the
  *        appropriate changes and decides the next state in the state machine.
  * @param GPIO_Pin: the pin that initiated the interrupt
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin == N_Btn_Pin && !NBtnPressed){
		if (!greenVertical && stayGreen && !SBtnPressed){	// green indefinitely in current state
			SBtnPressed = false;
			specificPin = 0x80;
		} else {
			specificPin = N_Blue;
			NBtnPressed = true;
			xSemaphoreGiveFromISR(NbinarySem, NULL);	// BlueLeds task will take
			if (greenVertical && SBtnPressed == false){
				if (stayGreen){
					NextState = DefaultState;
					waitTime = pedestrianDelay;
				}
			} else if (pinState(S_Blue)) {
				setPin(N_Blue);							// sync blinking with other leds
			}
		}
	}
	else if (GPIO_Pin == S_Btn_Pin && !SBtnPressed){
		if (greenVertical && stayGreen && !NBtnPressed){
			SBtnPressed = false;
			specificPin = 0x80;
		} else {
			specificPin = S_Blue;
			SBtnPressed = true;
			xSemaphoreGiveFromISR(NbinarySem, NULL);
			if (!greenVertical && NBtnPressed == false){
				if (stayGreen){
					NextState = DefaultState;
					waitTime = pedestrianDelay;
				}

			} else if (pinState(N_Blue)) {
				setPin(S_Blue);
			}
		}
	}
	else {
		carsVertical = HAL_GPIO_ReadPin(N_Car_GPIO_Port, N_Car_Pin)==0 || HAL_GPIO_ReadPin(S_Car_GPIO_Port, S_Car_Pin)==0;
		carsHorizontal = HAL_GPIO_ReadPin(E_Car_GPIO_Port, E_Car_Pin)==0 || HAL_GPIO_ReadPin(W_Car_GPIO_Port, W_Car_Pin)==0;
		if (!NBtnPressed && !SBtnPressed && State != YellowState){
			if (carsVertical){
				if (!carsHorizontal)
					NextState = greenVertical ? Start : YellowState;
				else {
					waitTime = redDelayMax;
					NextState = DefaultState;
				}
			}
			else if (carsHorizontal)
				NextState = greenVertical ? YellowState : Start;
			else {
				waitTime = greenDelay;
				NextState = DefaultState;
			}
			stayGreen = NextState == DefaultState ? false : true;
		}
	}
}
