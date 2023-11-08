// sensor.c
// sensor functions

#include "sensor.h"
#include "STM32L432KC_ADC.h"
#include "STM32L432KC_GPIO.h"

long map_range(long x, long in_min, long in_max, long out_min, long out_max)
{
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

  for (int i = 0; i < HORIZONTAL_BITS; i++) {
    digitalWrite(sensor_cfg->row_pins[i], (y >> i) & 1);
  }
}

uint8_t adcToBrightness(SENSOR_CFG_TypeDef *sensor_cfg, uint16_t adcVal) {
    return map_range(adcVal, 0, 65536, sensor_cfg->min_light, sensor_cfg->max_light);
}