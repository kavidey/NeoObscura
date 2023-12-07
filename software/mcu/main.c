/*
File: main.c
Author: Kavi Dey
Email: kdey@hmc.edu
Date: 10/30/23

Main file for NeoObscura
*/

#include <stdint.h>
#include <stdio.h>

#include <stm32l432xx.h>

#include "lib/STM32L432KC.h"
#include "lib/STM32L432KC_ADC.h"
#include "lib/STM32L432KC_FLASH.h"
#include "lib/STM32L432KC_GPIO.h"
#include "lib/STM32L432KC_RCC.h"
#include "lib/STM32L432KC_USART.h"

#include "lib/debayer.h"
#include "lib/qoi.h"
#include "lib/sensor.h"

#include "main.h"

#define COLOR_MODE
#define QOI // (requires color mode)

SENSOR_CFG_TypeDef sensor_cfg = {{ROW_0, ROW_1, ROW_2, ROW_3, ROW_4},
                                 {COL_0, COL_1, COL_2, COL_3, COL_4, COL_5},
                                 0.1,
                                 -0.1};
int x_coord = 0;
int y_coord = 0;
int frame_done = 0;

pixel_buf_Type pixel_buf_a;
pixel_buf_Type pixel_buf_b;
int curr_buff = 0;

#ifdef COLOR_MODE
color_pixel_buf_Type color_pixel_buf;
#ifdef QOI
encoded_image_Type encoded_image_buf;
#endif
#endif

char tempString[32];
USART_TypeDef *USART_LAPTOP;

void ADC1_IRQHandler(void) {
  // Clear the interrupt flag
  ADC1->ISR |= ADC_ISR_EOC;

  if (!frame_done) {
    // Read the ADC value
    pixel_buf_Type *pixel_buf = (curr_buff ? &pixel_buf_a : &pixel_buf_b);
    (*pixel_buf)[y_coord][x_coord] = adcToBrightness(&sensor_cfg, ADC1->DR);

    // Start the next conversion
    x_coord++;
    if (x_coord == HORIZONTAL_RESOLUTION) {
      x_coord = 0;
      y_coord++;
    }
    if (y_coord == VERTICAL_RESOLUTION) {
      y_coord = 0;
      x_coord = 0;
      frame_done = 1;
      return;
    }
    selectPixel(&sensor_cfg, col_mapping[x_coord], y_coord);
    ADC1->CR |= ADC_CR_ADSTART;
  }
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

#ifdef QOI
  // Setup FPGA
  initQOI(FPGA_RESET);
#endif

  // Setup UART
  USART_LAPTOP = initUSART(USART2_ID, 921600);

  // Setup ADC
  initADC(ADC_12BIT_RESOLUTION);
  initChannel(ANALOG_IN_ADC_CHANNEL);

  // Enable end of conversion interrupt
  NVIC->ISER[0] |= (1 << 18);
  ADC1->IER |= ADC_IER_EOCIE;

  // Start the first conversion
  selectPixel(&sensor_cfg, col_mapping[0], 0);
  ADC1->CR |= ADC_CR_ADSTART;

  while (1) {
    if (frame_done) {
      // Switch to the other buffer
      curr_buff ^= 1;
      pixel_buf_Type *pixel_buf = (!curr_buff ? &pixel_buf_a : &pixel_buf_b);

      // Get ready for the next conversion
      frame_done = 0;
      y_coord = 0;
      x_coord = 0;
      selectPixel(&sensor_cfg, col_mapping[x_coord], y_coord);
      ADC1->CR |= ADC_CR_ADSTART;

#ifdef COLOR_MODE
      // Debayer the image
      debayer(pixel_buf, &color_pixel_buf);
#ifdef QOI
      // Compress the image
      encodeImage(&color_pixel_buf, &encoded_image_buf, FPGA_RESET);
#endif
#endif

#ifndef QOI
      for (int i = VERTICAL_RESOLUTION - 1; i >= 0; i--) {
        for (int j = 0; j < HORIZONTAL_RESOLUTION; j++) {
#ifdef COLOR_MODE
          rgba_t px = color_pixel_buf[i][j];
          sprintf(tempString, "%02hhx%02hhx%02hhx", px.r, px.g, px.b);
#else
          sprintf(tempString, "%02hhx", (*pixel_buf)[i][j]);
#endif
          sendString(USART_LAPTOP, tempString);
        }
      }
      sendString(USART_LAPTOP, "\n");
#else
      for (int i = 0; i < encoded_image_buf.size; i++) {
        // sprintf(tempString, "%02hhx", encoded_image_buf.imageData[i]);
        // sendString(USART_LAPTOP, tempString);
        sendChar(USART_LAPTOP, encoded_image_buf.imageData[i]);
      }
#endif
    }
  }
}
