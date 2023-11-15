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
#include <stdint.h>
#include <stdio.h>
#include <stm32l432xx.h>

SENSOR_CFG_TypeDef sensor_cfg = {{ROW_0, ROW_1, ROW_2, ROW_3, ROW_4},
                                 {COL_0, COL_1, COL_2, COL_3, COL_4, COL_5},
                                 0,
                                 1000};
int x_coord = 0;
int y_coord = 0;
int frame_done = 0;

typedef uint8_t pixel_buf_Type[HORIZONTAL_RESOLUTION][VERTICAL_RESOLUTION];

pixel_buf_Type pixel_buf_a;
pixel_buf_Type pixel_buf_b;
int curr_buff = 0;

char tempString[32];
USART_TypeDef *USART_LAPTOP;

void ADC1_IRQHandler(void) {
  // Clear the interrupt flag
  ADC1->ISR |= ADC_ISR_EOC;

  // Read the ADC value
  pixel_buf_Type *pixel_buf = (curr_buff ? &pixel_buf_a : &pixel_buf_b);
  (*pixel_buf)[x_coord][y_coord] = ADC1->DR / 256;

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

  /// Setup Peripherals ///`
  // Setup sensor
  setupSensor(&sensor_cfg);

  // Setup UART
  USART_LAPTOP = initUSART(USART2_ID, 115200);

  // Setup ADC
  initADC(ADC_12BIT_RESOLUTION);
  initChannel(ANALOG_IN_ADC_CHANNEL);

  // Enable end of conversion interrupt
  NVIC->ISER[0] |= (1 << 18);
  ADC1->IER |= ADC_IER_EOCIE;

  // Start the first conversion 
  ADC1->CR |= ADC_CR_ADSTART;

  // while (1) {
  //   for (int i = 0; i < 29; i++) {
  //     selectPixel(&sensor_cfg, i, i);

  //     sprintf(tempString, "%i\n", i);
  //     sendString(USART_LAPTOP, tempString);

  //     volatile int x = 1000000;
  //     while (x-- > 0)
  //       __asm("nop");
  //   }
  // }

  // digitalWrite(ROW_0, 1);
  // digitalWrite(ROW_1, 1);
  // digitalWrite(ROW_2, 1);
  // digitalWrite(ROW_3, 1);
  // digitalWrite(ROW_4, 1);
  // digitalWrite(COL_0, 1);
  // digitalWrite(COL_1, 1);
  // digitalWrite(COL_2, 1);
  // digitalWrite(COL_3, 1);
  // digitalWrite(COL_4, 1);
  // digitalWrite(COL_5, 1);
  // selectPixel(&sensor_cfg, 0, 0);

  while (1) {
    if (frame_done) {
      // Switch to the other buffer
      curr_buff ^= 1;
      pixel_buf_Type *pixel_buf = (!curr_buff ? &pixel_buf_a : &pixel_buf_b);


      // Get ready for the next conversion
      frame_done = 0;
      ADC1->CR |= ADC_CR_ADSTART;

      for (int j = VERTICAL_RESOLUTION-1; j >= 0; j--) {
        for (int i = 0; i < HORIZONTAL_RESOLUTION; i++) {
          sprintf(tempString, "%02X", (*pixel_buf)[i][j]);
          sendString(USART_LAPTOP, tempString);
        }
      }
      sendString(USART_LAPTOP, "\n");
    }
  }
}
