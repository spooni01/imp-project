/********************************************
 *	Project: 	IMP project					*
 * 	Author:		Adam Ližièiar (xlizic00)	*
 ********************************************/

#include <stdio.h>
#include <string.h>
#include "MK60D10.h"


// Variables
unsigned int sensorMeasuresDistance = 1;
unsigned int distance = 0;					// Distance from sensor in centimeters
unsigned long long int senzorDistance = 0;	// Distance from sensor without format
int numOfDigitOnDisplay = 3;

// Define constants
#define NUM_FIRST  0x100
#define NUM_SECOND 0x200
#define NUM_THIRD  0x1000
#define NUM_FOURTH 0x2000

#define LINE_UP        		0x800
#define LINE_LEFT_UP   		0x80
#define LINE_RIGHT_UP  		0x200
#define LINE_CENTER     	0x8000
#define LINE_LEFT_DOWN 0x40
#define LINE_RIGHT_DOWN 	0x4000
#define LINE_BOTTOM     	0x400
#define LINE_DOT        	0x100

#define PIN_SEND   	0x1000000
#define PIN_RECEIVE 0x2000000


// Macros for manipulating bit-level registers
#define GPIO_PIN_MASK  0x1Fu
#define GPIO_PIN(x)    ((1U) << ((x) & GPIO_PIN_MASK))


