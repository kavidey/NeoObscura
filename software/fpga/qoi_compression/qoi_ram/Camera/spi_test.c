/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Generic application start

*/

#include <stdio.h>
#include <stm32l432xx.h>
#include "/Users/neilchulani/Documents/Polarbear/College/Classes/Engineering/MicroPs/HMC-E155/lab/lab07/lab07_mcu/lib/STM32L432KC.h"
#include <time.h>

#define spiIn     PA4
#define spiOut    PA5

struct pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

int main(void) {
  volatile int i = 0;

  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN);

  

  pinMode(spiIn, GPIO_OUTPUT);
  pinMode(spiOut, GPIO_OUTPUT); 

  digitalWrite(spiIn, 0);
  digitalWrite(spiOut, 0); 

  initSPI(4, 0, 1);
  struct pixel test = {0xA9, 0xBC, 0x12, 0xAB};

  

  digitalWrite(spiIn, 1);
  while (i < 50) {i++;}
  i = 0;
  spiSendReceive(test.r);
  spiSendReceive(test.g);
  spiSendReceive(test.b);
  spiSendReceive(test.a);
  while(SPI1->SR & SPI_SR_BSY);
  digitalWrite(spiIn, 0);

  digitalWrite(spiOut, 1);
  while (i < 50) {i++;}
  i = 0;
  uint8_t recieved = spiSendReceive(0x0F);
  while(SPI1->SR & SPI_SR_BSY);
  digitalWrite(spiOut, 0);

  digitalWrite(spiIn, 1);
  while (i < 50) {i++;}
  i = 0;
  spiSendReceive(0xBB);
  spiSendReceive(255);
  spiSendReceive(255);
  spiSendReceive(0x98);
  while(SPI1->SR & SPI_SR_BSY);
  digitalWrite(spiIn, 0);

  digitalWrite(spiOut, 1);
  while (i < 50) {i++;}
  i = 0;
  uint8_t recieved2 = spiSendReceive(0x0F);
  while(SPI1->SR & SPI_SR_BSY);
  digitalWrite(spiOut, 0);

  digitalWrite(spiOut, 1);
  while (i < 50) {i++;}
  i = 0;
  uint8_t recieved3 = spiSendReceive(0x0F);
  while(SPI1->SR & SPI_SR_BSY);
  digitalWrite(spiOut, 0);

  while(1);



}
