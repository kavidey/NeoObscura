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

SENSOR_CFG_TypeDef sensor_cfg = {{ROW_0, ROW_1, ROW_2, ROW_3, ROW_4},
                                 {COL_0, COL_1, COL_2, COL_3, COL_4, COL_5},
                                 0,
                                 1000};
int x_coord = 0;
int y_coord = 0;
int frame_done = 0;
uint8_t pixels[HORIZONTAL_RESOLUTION][VERTICAL_RESOLUTION];

void ADC1_IRQHandler(void) {
  // Clear the interrupt flag
  ADC1->ISR |= ADC_ISR_EOC;

  // Read the ADC value
  pixels[x_coord][y_coord] = ADC1->DR / 256;

  // Start the next conversion
  x_coord++;
  if (x_coord == HORIZONTAL_RESOLUTION) {
    x_coord = 0;
    y_coord++;
  }
  if (y_coord == VERTICAL_RESOLUTION) {
    y_coord = 0;
    frame_done = 1;
    return;
  }
  selectPixel(&sensor_cfg, x_coord, y_coord);
  ADC1->CR |= ADC_CR_ADSTART;
}

int main(void) {
  // Configure flash and PLL at 80 MHz
  configureFlash();
  configureClock();

  // Enable interrupts
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  __enable_irq();

  /// Setup GPIO ///
  // Enable Clock for GPIO Ports A, B, and C
  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  // Set up analog input
  pinMode(ANALOG_IN, GPIO_ANALOG);

  // Setup sensor
  setupSensor(&sensor_cfg);

  // Setup UART
  USART_TypeDef *USART_LAPTOP = initUSART(USART2_ID, 19200);

  // Setup ADC
  initADC(ADC_12BIT_RESOLUTION);
  initChannel(ANALOG_IN_ADC_CHANNEL);

  // Enable end of conversion interrupt
  NVIC->ISER[0] |= (1 << 18);
  ADC1->IER |= ADC_IER_EOCIE;

  char tempString[32];

  // Start the first conversion
  ADC1->CR |= ADC_CR_ADSTART;

  while (1) {
    if (frame_done) {
      for (int i = 0; i < HORIZONTAL_RESOLUTION; i++) {
        for (int j = 0; j < VERTICAL_RESOLUTION; j++) {
          sprintf(tempString, "%02X", pixels[i][j]);
          sendString(USART_LAPTOP, tempString);
        }
      }
      sendString(USART_LAPTOP, "\n");

      // Get ready for the next conversion
      frame_done = 0;
      ADC1->CR |= ADC_CR_ADSTART;
    }
  }
}
