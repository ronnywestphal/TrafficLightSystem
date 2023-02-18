# TrafficLightSystem
The objective was to write a traffic light control system for a 4-way stop 
with two pedestrian crossings. The system is implemented with FreeRTOS on a 
NUCLEO-L476RG with a traffic light shield.

<p align="center">
  <img src="https://user-images.githubusercontent.com/84048902/219415244-5f02771e-b9cc-4208-873b-b8218f1b7cb7.png"
height="400" />
</p>

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


## Traffic-Lights-API
The Traffic-Lights-API handles the communication between the application 
and the HAL and defines the mapping of the components on the traffic light shield. 
The requirements of the API are:
- set/clear LED pattern
- set/clear specific LED
- clear all LEDs except blue LEDs
- toggle LED
- read specific LED
- transmit LED pattern

The LEDs are controlled via three 8-bit shift registers and the data is 
transmitted using the SPI-interface. Each shift register is mapped
to an index in an array of three 8-bit unsigned integers.

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
state machine. The GPIO pins connected to the buttons and switches are 
initiated as external interrupts and all the code related to the cars and pedestrians is
handled by the same callback function.
<p align="center">
  <img src= "https://user-images.githubusercontent.com/84048902/219875067-b4569188-017b-44f7-9052-f8bdf16611c4.png"
   height="500" />
</p>


## System Diagram

<p align="center">
  <img src="https://user-images.githubusercontent.com/84048902/219417126-796bb143-3587-4977-af61-eae045868214.jpg"
height="600" />
</p>
