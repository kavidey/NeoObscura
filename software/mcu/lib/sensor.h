/*
File: sensor.h
Author(s): Kavi Dey
Email(s): kdey@hmc.edu
Date: 12/7/23

Header for interfacing with sensor
*/

#ifndef sensor_H
#define sensor_H

#include <stdint.h> // Include stdint header
#include "STM32L432KC_GPIO.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define HORIZONTAL_BITS 6
#define HORIZONTAL_RESOLUTION 40
#define VERTICAL_BITS 5
#define VERTICAL_RESOLUTION 30
#define NUM_PIXELS HORIZONTAL_RESOLUTION*VERTICAL_RESOLUTION

typedef struct {
    int row_pins[VERTICAL_BITS];
    int col_pins[HORIZONTAL_BITS];
    float alpha;
    float beta;
} SENSOR_CFG_TypeDef;

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

/* Sets up all the IO necessary to interface with the sensor
 *    -- sensor_cfg: sensor configuration struct*/ 
void setupSensor(SENSOR_CFG_TypeDef* sensor_cfg);

/* Sets up the muxes to select the pixel at the given x,y coordinate
 *    -- sensor_cfg: sensor configuration struct
 *    -- x: pixel x coord
 *    -- y: pixel y coord*/ 
void selectPixel(SENSOR_CFG_TypeDef* sensor_cfg, int x, int y);

/* Converts a raw ADC reading to a 0-255 brightness value using alpha-beta contrast / brightness adjustment
 *    -- sensor_cfg: sensor configuration struct
 *    -- adcVal: raw adc reading
 *
 * returns: pixel brightness*/
uint8_t adcToBrightness(SENSOR_CFG_TypeDef *sensor_cfg, uint16_t adcVal);

/* Remaps a number from one range to another
 *    -- x: number to remap
 *    -- in_min: lower bound of starting range
 *    -- in_max: upper bound of starting range
 *    -- out_min: lower bound of ending range
 *    -- out_max: upper bound of ending range
 *
 * returns: remapped number*/ 
long map_range(long x, long in_min, long in_max, long out_min, long out_max);

#endif