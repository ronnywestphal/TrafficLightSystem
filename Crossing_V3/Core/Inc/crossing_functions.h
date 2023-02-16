
#include "gpio_functions.h"
#include "stdbool.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void toggleInterruptNextState();
void yellowLeds();
void checkNextState();
void nextDirection();
void blueStateToggle();
void endBlueState();
void afterBlueState();


