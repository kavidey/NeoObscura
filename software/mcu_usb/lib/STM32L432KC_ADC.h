// STM32F401RE_ADC.h
// Header for ADC functions

#ifndef STM32L4_ADC_H
#define STM32L4_ADC_H

#include <stdint.h> // Include stdint header
#include "STM32L432KC_GPIO.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define ADC_12BIT_RESOLUTION 0b00
#define ADC_10BIT_RESOLUTION 0b01
#define ADC_8BIT_RESOLUTION 0b10
#define ADC_6BIT_RESOLUTION 0b11

#define ADC_PA0 5  // ADC1_IN5
#define ADC_PA1 6  // ADC1_IN6
#define ADC_PA2 7  // ADC1_IN7
#define ADC_PA3 8  // ADC1_IN8
#define ADC_PA4 9  // ADC1_IN9
#define ADC_PA5 10 // ADC1_IN10
#define ADC_PA6 11 // ADC1_IN11
#define ADC_PA7 12 // ADC1_IN12

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void initADC(int resolution);

void initChannel(int channel);

uint16_t readADC();

#endif