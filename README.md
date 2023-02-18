# TrafficLightSystem
The objective was to write a traffic light control system for a 4-way stop 
with two pedestrian crossings. The system is implemented with and without RTOS on a 
NUCLEO-L476RG with a traffic light shield.

<p align="center">
  <img src="https://user-images.githubusercontent.com/84048902/219415244-5f02771e-b9cc-4208-873b-b8218f1b7cb7.png"
height="400" />
</p>


## Traffic-Lights-API
The Traffic-Lights-API handles the communication between the ap-
plication and the HAL and defines the mapping of the components
on the traffic light shield. The requirements of the API are:
- set/clear LED pattern
- set/clear specific LED
- clear all LEDs except blue LEDs
- toggle LED
- read specific LED
- transmit LED pattern

Referencing the schematics and using the SPI-interface
to send 8-bit packages, MSB first, each shift register was mapped
to an index in an array of three 8-bit unsigned integers, which was
placed in a global struct so that all parts of the system could refer-
ence it.

## Functions-module
This module contains the functionality of the state machine that
defines the behaviour of the system. The tasks that the functions
needs to handle are:
- check the next LED pattern
- decide the next LED pattern
- decide which blue LEDs to toggle
- set the LED pattern to yellowState

## External Interrupt Callback
External interrupts were used in order to simplify the logic of the
state machine. All the code related to the cars and pedestrians were
added to the same callback function. The GPIO pins connected to
the buttons and switches are initiated as external interrupts and are
handled by the same callback function.

## FreeRTOS
The system was divided into three functionalities that needed to be
handled independently:
- a state machine
- toggle the blue LEDs
- contineously transmit LED pattern
The state machine handles most of the functionality. It is divided
in to three states:
- Start - sets the next LED pattern
- DefaultState - keeps the current LED pattern on for a defined time
- YellowState - sets the LED pattern to yellow for a defined time



<p align="center">
  <img src="https://user-images.githubusercontent.com/84048902/219417126-796bb143-3587-4977-af61-eae045868214.jpg"
height="600" />
</p>
