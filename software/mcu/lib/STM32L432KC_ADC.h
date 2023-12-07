/*
File: STM32F401RE_ADC.h
Author(s): Kavi Dey
Email(s): kdey@hmc.edu
Date: 11/9/23

Header for ADC functions
*/

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

/* Turns on and initializes the ADC
 *    -- resolution: (0b00-0b11) ADC resolution to use
 * Note: this does not setup any ADC channels, use `initChannel` to do that
 */
void initADC(int resolution);

/* Sets up a single ADC channel for conversoin
 *    -- channel: which channel to initialize (should be in the form ADC_PAX)
 * Notes:
 * The pin associated with that channel must be manually set to ANALOG_IN mode
 * This only works for 1 ADC channel at a time
 */
void initChannel(int channel);

/* Reads a single value from the enabled ADC Channel
 * returns: channel voltage
 * Note: this function is blocking, it is recommended to use the ADC conversion complete interrupt
 */
uint16_t readADC();

#endif