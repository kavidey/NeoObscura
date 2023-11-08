// sensor.h
// Header for sensor functions

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
    int min_light;
    int max_light;
} SENSOR_CFG_TypeDef;

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void setupSensor(SENSOR_CFG_TypeDef* sensor_cfg);
void selectPixel(SENSOR_CFG_TypeDef* sensor_cfg, int x, int y);
uint8_t adcToBrightness(SENSOR_CFG_TypeDef *sensor_cfg, uint16_t adcVal);

long map_range(long x, long in_min, long in_max, long out_min, long out_max);

#endif