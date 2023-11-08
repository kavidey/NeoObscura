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

void setupPins() {
  // Enable Clock for GPIO Ports A, B, and C
  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  // Set up analog input
  pinMode(ANALOG_IN, GPIO_ANALOG);

  // Setup Row and Column pins
  pinMode(ROW_0, GPIO_OUTPUT);
  pinMode(ROW_1, GPIO_OUTPUT);
  pinMode(ROW_2, GPIO_OUTPUT);
  pinMode(ROW_3, GPIO_OUTPUT);
  pinMode(ROW_4, GPIO_OUTPUT);

  pinMode(COL_0, GPIO_OUTPUT);
  pinMode(COL_1, GPIO_OUTPUT);
  pinMode(COL_2, GPIO_OUTPUT);
  pinMode(COL_3, GPIO_OUTPUT);

  pinMode(MUX1_EN, GPIO_OUTPUT);
  pinMode(MUX2_EN, GPIO_OUTPUT);
  pinMode(MUX3_EN, GPIO_OUTPUT);
}

char tempString[32];

int main(void) {
  // Configure flash and PLL at 80 MHz
  configureFlash();
  configureClock();

  // Setup GPIO
  setupPins();

  // Setup UART
  USART_TypeDef *USART_DEBUG = initUSART(USART2_ID, 9600);

  // Setup ADC
  initADC(ADC_12BIT_RESOLUTION);
  initChannel(ADC_PA0);

  sendString(USART_DEBUG, "DEBUG: ADC Initialized\n");

  while (1) {
    sprintf(tempString, "%d\n", readADC());
    sendString(USART_DEBUG, tempString);
  }
}
