/**
 **************************************************************
 * @brief  GPIO API for the traffic lights
 * @file   gpio_functions.c
 * @author Ronny Westphal
 * @date   16-december-2022
 * @brief  Functions to set/clear specific/multiple GPIO-pins
 **************************************************************
 */
/* Includes --------------------------------------------------*/
#include "gpio_functions.h"

struct LedRegs LedRegister;
extern SPI_HandleTypeDef hspi3;

/**
 * @brief sets all LEDs in a specified pattern
 * @param ledPattern: an array that contains the pattern that specifies a green-lights-state or yellow-lights-state
 */
void gpio_setMultipleLights(const uint8_t ledPattern[]){
	for (int i=0; i<3; i++){
		LedRegister.ledReg[i] |= ledPattern[i];
	}
}

/**
 * @brief turns of all LEDs
 */
void gpio_clearAllPins(){
	for (int i=0; i<3; i++){
		LedRegister.ledReg[i] = 0U;
	}
}

/**
 * @brief turns off all except the blue LEDs
 */
void gpio_clearAllNotBlue(){
	LedRegister.ledReg[0] = 0U;
	LedRegister.ledReg[1] &= 1U<<S_Blue%8;
	LedRegister.ledReg[2] &= 1U<<N_Blue%8;
}

/**
 * @brief toggles the specified LED
 * @param pinNumber: the pin for the specified LED
 */
void gpio_togglePin(GpioPinPosition pinNumber){
	if (pinState(pinNumber))
		clearPin(pinNumber);
	else setPin(pinNumber);

}

/**
 * @brief checks if the specified LED is on or off
 * @param pinNumber: the pin for the specified LED
 * @return true if the LED is ON, false otherwise
 */
bool pinState(GpioPinPosition pinNumber){
	return (LedRegister.ledReg[pinNumber/8] & (1U << pinNumber % 8)) >> pinNumber % 8 != 0;
}

/**
 * @brief turns off the specified LED
 * @param pinNumber: the pin for the specified LED
 */
void clearPin(GpioPinPosition pinNumber){
	LedRegister.ledReg[pinNumber/8] &= ~(1U << pinNumber % 8);
}

/**
 * @brief turns on the specified LED
 * @param pinNumber: the pin for the specified LED
 */
void setPin(GpioPinPosition pinNumber){
	LedRegister.ledReg[pinNumber/8] |= (1U << pinNumber % 8);
}

/**
 * @brief transmits the current state of the LEDs to the shift registers
 */
void LED_pattern_transmit()
{
	HAL_GPIO_WritePin(Enable595_GPIO_Port, Enable595_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(STCP595_GPIO_Port, STCP595_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Reset595_GPIO_Port, Reset595_Pin, GPIO_PIN_SET);

	HAL_SPI_Transmit(&hspi3, LedRegister.ledReg, 3, 100);
	HAL_GPIO_WritePin(STCP595_GPIO_Port, STCP595_Pin, GPIO_PIN_SET);
}
