#ifndef GPIO_FUNCTIONS_H_
#define GPIO_FUNCTIONS_H_
#include "stdint.h"

/* Struct that holds the 3*8 bits that gets transmitted to the shift registers */
struct LedRegs {
	uint8_t ledReg[3];
};

#define GLOBAL extern
GLOBAL struct LedRegs LedRegister;

/* Define the LED-patterns that are always active */
static const uint8_t verticalGreen[] = {0x21,0x0C,0x11};
static const uint8_t horizontalGreen[] = {0x0C,0x11,0x0C};
static const uint8_t yellowState[] = {0x12,0x0A,0x0A};

/* Define the offsets for the shift registers */
#define REG1 0
#define REG2 8
#define REG3 16

/* Define the pin positions for the traffic lights */
typedef enum _GpioPinPosition {
	NE_Car = (REG1),
	NW_Car = ((REG1+3)),
	SW_Car = (REG3),
	SE_Car = (REG2),
	N_Ped = (REG2+3),
	S_Ped = (REG3+3),
	N_Blue = (REG2+5),
	S_Blue = (REG3+5)

} GpioPinPosition;

#endif

/* Includes -------------------------------------------------------------*/
#include "spi.h"
#include "stdbool.h"

/* Function declarations ------------------------------------------------*/
void gpio_setMultipleLights(const uint8_t ledPattern[]);
void gpio_clearAllPins();
void gpio_clearAllNotBlue();
void gpio_togglePin(GpioPinPosition pinNumber);
bool pinState(GpioPinPosition pinNumber);
void clearPin(GpioPinPosition pinNumber);
void setPin(GpioPinPosition pinNumber);
void LED_pattern_transmit();
