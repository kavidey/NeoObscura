/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Generic application start

*/

#include <stdio.h>
#include <stdlib.h>
#include "lib/STM32L432KC.h"
#include "lib/STM32L432KC_USB.h"
#include "lib/STM32L432KC_FLASH.h"
#include "lib/STM32L432KC_GPIO.h"
#include "lib/STM32L432KC_RCC.h"
#include "lib/STM32L432KC_USART.h"
#include <stdint.h>
#include <stdio.h>
#include <stm32l432xx.h>

char tempString[32];
USART_TypeDef *USART_LAPTOP;

int main(void) {
  // Configure flash and PLL at 80 MHz
  configureFlash();
  configureClock();

  // Enable interrupts
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  __enable_irq();

  // Setup UART
  USART_LAPTOP = initUSART(USART2_ID, 115200);

  // Setup USB
  initUSB();

  int i;

  for (i = 0; i < 100; i++) {
    sprintf(tempString, "Hello World %d!\n", i);
    sendString(USART_LAPTOP, tempString);
  }
  do {
    i++;
  } while (1);
}

/*************************** End of file ****************************/
