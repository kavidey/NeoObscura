/*
File: main.h
Author: Kavi Dey
Email: kdey@hmc.edu
Date: 10/30/23

Main header file for NeoObscura
*/

/// Pin definitions
#define ANALOG_IN PA3
#define ANALOG_IN_ADC_CHANNEL ADC_PA3

// Row Address Pins
#define ROW_0 PB1
#define ROW_1 PB7
#define ROW_2 PA12
#define ROW_3 PB5
#define ROW_4 PB3

// Column Address Pins
#define COL_0 PA10
#define COL_1 PB0
#define COL_2 PB6
#define COL_3 PA8
#define COL_4 PC14 // replace this later, will connect to the MCU (also PC14 doesn't work for some reason)
#define COL_5 PA6 // replace this later, will connect to the MCU