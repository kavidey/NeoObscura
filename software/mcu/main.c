/*
File: main.c
Author: Kavi Dey
Email: kdey@hmc.edu
Date: 10/30/23

Main file for NeoObscura
*/

#include "main.h"
#include "lib/STM32L432KC.h"
#include "lib/STM32L432KC_ADC.h"
#include "lib/STM32L432KC_FLASH.h"
#include "lib/STM32L432KC_GPIO.h"
#include "lib/STM32L432KC_RCC.h"
#include "lib/STM32L432KC_USART.h"
#include "lib/sensor.h"
#include <stdio.h>
#include <stm32l432xx.h>

int main(void) {
  // Configure flash and PLL at 80 MHz
  configureFlash();
  configureClock();

  /// Setup GPIO ///
  // Enable Clock for GPIO Ports A, B, and C
  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  // Set up analog input
  pinMode(ANALOG_IN, GPIO_ANALOG);

  SENSOR_CFG_TypeDef sensor_cfg = {{ROW_0, ROW_1, ROW_2, ROW_3, ROW_4},
                                   {COL_0, COL_1, COL_2, COL_3, COL_4, COL_5},
                                   0,
                                   1000};

  setupSensor(&sensor_cfg);

  // Setup UART
  USART_TypeDef *USART_LAPTOP = initUSART(USART2_ID, 19200);

  // Setup ADC
  initADC(ADC_12BIT_RESOLUTION);
  initChannel(ADC_PA0);

  // sendString(USART_DEBUG, "[DEBUG] ADC Initialized\n");

  char tempString[32];
  uint8_t pixels[HORIZONTAL_RESOLUTION][VERTICAL_RESOLUTION];

  while (1) {
    for (int i = 0; i < HORIZONTAL_RESOLUTION; i++) {
      for (int j = 0; j < VERTICAL_RESOLUTION; j++) {
        sprintf(tempString, "%02X", adcToBrightness(&sensor_cfg, readADC()));
        sendString(USART_LAPTOP, tempString);
      }
    }

    sendString(USART_LAPTOP, "\n");
  }
}
