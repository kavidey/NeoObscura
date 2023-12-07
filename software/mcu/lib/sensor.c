/*
File: sensor.c
Author(s): Kavi Dey
Email(s): kdey@hmc.edu
Date: 12/7/23

Source code for interfacing with sensor
*/

#include "sensor.h"
#include "STM32L432KC_ADC.h"
#include "STM32L432KC_GPIO.h"

long map_range(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setupSensor(SENSOR_CFG_TypeDef *sensor_cfg) {
  // Setup all horizontal and vertical pins as outputs
  for (int i = 0; i < HORIZONTAL_BITS; i++) {
    pinMode(sensor_cfg->col_pins[i], GPIO_OUTPUT);
  }

  for (int i = 0; i < VERTICAL_BITS; i++) {
    pinMode(sensor_cfg->row_pins[i], GPIO_OUTPUT);
  }
}

void selectPixel(SENSOR_CFG_TypeDef *sensor_cfg, int x, int y) {
  // Write the selected row and column to the pins
  for (int i = 0; i < HORIZONTAL_BITS; i++) {
    digitalWrite(sensor_cfg->col_pins[i], (x >> i) & 1);
  }

  for (int i = 0; i < VERTICAL_BITS; i++) {
    digitalWrite(sensor_cfg->row_pins[i], (y >> i) & 1);
  }
}

uint8_t adcToBrightness(SENSOR_CFG_TypeDef *sensor_cfg, uint16_t adcVal) {
  float val = ((float)adcVal) / 65536.0;
  val = (val * (1 + sensor_cfg->alpha) + sensor_cfg->beta);

  // Clamp output
  if (val < 0) {
    val = 0;
  } else if (val > 1) {
    val = 1;
  }
  
  return (uint8_t)(val * 255.0);
}