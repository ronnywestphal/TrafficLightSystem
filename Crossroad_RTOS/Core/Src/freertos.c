/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    : freertos.c
  * @author  : Ronny Westphal
  * @date    : 16-december-2022
  * @brief   : Freertos implementation of the Traffic Lights Project.
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include "gpio_functions.h"
#include "crossing_functions.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern states State, NextState;
uint32_t waitTime = greenDelay;

GpioPinPosition specificPin = 0x80;

bool carsVertical = false;
bool carsHorizontal = false;
bool greenVertical = false;
bool NBtnPressed = false;
bool SBtnPressed = false;
bool stayGreen = false;

SemaphoreHandle_t NbinarySem;
SemaphoreHandle_t mutex;
uint32_t debugCounter = 0;
/* USER CODE END Variables */

/* Definitions for MainTask */
osThreadId_t MainTaskHandle;
const osThreadAttr_t MainTask_attributes = {
  .name = "MainTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TransmitTask */
osThreadId_t TransmitTaskHandle;
const osThreadAttr_t TransmitTask_attributes = {
  .name = "TransmitTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for BlueLeds */
osThreadId_t BlueLedsHandle;
const osThreadAttr_t BlueLeds_attributes = {
  .name = "BlueLeds",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};


/* Private function prototypes -----------------------------------------------*/

void StartDefaultTask(void *argument);
void StateMachine(void *argument);
void TransmitData(void *argument);
void StartBlueLeds(void *argument);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  */
void MX_FREERTOS_Init(void) {
  /* Create the thread(s) */
  /* creation of MainTask */
  MainTaskHandle = osThreadNew(StateMachine, NULL, &MainTask_attributes);
  /* creation of TransmitTask */
  TransmitTaskHandle = osThreadNew(TransmitData, NULL, &TransmitTask_attributes);
  /* creation of BlueLeds */
  BlueLedsHandle = osThreadNew(StartBlueLeds, NULL, &BlueLeds_attributes);
}

/* USER CODE BEGIN Header_MainTask */
/**
* @brief Function implementing the MainTask thread.
* 		 State machine for the Traffic Lights Project.
*/
/* USER CODE END Header_MainTask */
void StateMachine(void *argument)
{
  /* USER CODE BEGIN CV_leds */
	NextState = Start;
	uint32_t localCounter = 0;

	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(10);
	xLastWakeTime = xTaskGetTickCount();

	for(;;){
		State = NextState;
		localCounter++;
		debugCounter = 1;
		switch (State){
		case Start:
			if (!stayGreen){
				nextDirection();
				NextState = DefaultState;
			}
			else if (localCounter >= waitTime-yellowDelay)		// reset timer after maximum wait time
				localCounter = 0;
			break;

		case DefaultState:
			if (localCounter >= waitTime-yellowDelay)
				NextState = YellowState;
			break;

		case YellowState:
			if (!pinState(S_Ped) || !pinState(N_Ped)){
				yellowLeds();
				localCounter = 0;
			}
			if (localCounter >= yellowDelay){
				localCounter = 0;
				checkNextState();
				if (NBtnPressed || SBtnPressed)
					nextBlueLedsAction();
			}
			break;
		}
		debugCounter=0;
		vTaskDelayUntil( &xLastWakeTime, xPeriod );
	}
  /* USER CODE END MainTask */
}

/* USER CODE BEGIN Header_TransmitData */
/**
* @brief Function implementing the TransmitTask thread.
* 		 Transmits the data stored in LedRegs every 100ms if the data is updated
*/
/* USER CODE END Header_TransmitData */
void TransmitData(void *argument)
{
  /* USER CODE BEGIN TransmitData */
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(100);
	xLastWakeTime = xTaskGetTickCount();

	uint8_t previousLEDPattern[3];

	for(;;)
	{
		for (uint8_t i=0; i<3; i++){
			if (LedRegister.ledReg[i] != previousLEDPattern[i]){
				LED_pattern_transmit();
				for (uint8_t i=0; i<3; i++){
					previousLEDPattern[i] = LedRegister.ledReg[i];
				}
			}
		}
		vTaskDelayUntil( &xLastWakeTime, xPeriod );
  	}
  /* USER CODE END TransmitData */
}

/* USER CODE BEGIN Header_BlueLeds */
/**
* @brief Function implementing the BlueLeds thread.
* 		 Starts blinking when the semaphore is given from the ISR
*/
/* USER CODE END Header_BlueLeds */
void StartBlueLeds(void *argument)
{
  /* USER CODE BEGIN BlueLeds */
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(toggleFreq);
	xLastWakeTime = xTaskGetTickCount();

	NbinarySem = xSemaphoreCreateBinary();

	for(;;)
	{
		if (xSemaphoreTake(NbinarySem, (TickType_t)0)==pdTRUE){
			blueStateToggle();
			xSemaphoreGive(NbinarySem);
		}
		vTaskDelayUntil( &xLastWakeTime, xPeriod );
	}
  /* USER CODE END BlueLeds */
}
