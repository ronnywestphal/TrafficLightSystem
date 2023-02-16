/**
 ****************************************************************************
 * @brief   Test functions for the GPIO API
 * @file    test.c
 * @author  Ronny Westphal
 * @date    16-december-2022
 ****************************************************************************
 */

#include "test.h"

void testing(void){

	while (1){
		testClearingAndPatterns();

	}
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

