/*
File: main.c
Author: Kavi Dey
Email: kdey@hmc.edu
Date: 10/30/23

Main file for NeoObscura
*/

#include "lib/STM32L432KC.h"
#include "lib/STM32L432KC_FLASH.h"
#include "lib/STM32L432KC_GPIO.h"
#include "lib/STM32L432KC_RCC.h"
#include "lib/sensor.h"
#include <stdio.h>
#include <stm32l432xx.h>

int main(void) {
    // Configure flash and PLL at 80 MHz
    configureFlash();
    configureClock();

    // Setup GPIO
    gpioEnable(GPIO_PORT_A);
    gpioEnable(GPIO_PORT_B);
    gpioEnable(GPIO_PORT_C);
}
