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
#define ROW_3 PA5
#define ROW_4 PA9

// Column Address Pins
#define COL_0 PA10
#define COL_1 PB0
#define COL_2 PB6
#define COL_3 PA8
#define COL_4 PA7
#define COL_5 PA6

// SPI Communication Pins
#define FPGA_RESET PA11

// Mapping between what column the MCU wants to access and what number that actually is
// The column wires are not plugged in in order due to wire length issues
const int col_mapping[40] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                             10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                             20, 21, 22, 23, 24, 25, 46, 45, 44, 43,
                             42, 41, 40, 39, 38, 37, 36, 35, 34, 33};
