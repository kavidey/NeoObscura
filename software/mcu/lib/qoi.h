/*
File: qoi.h
Author(s): Neil Chulani, Kavi Dey
Email(s): nchulani@hmc.edu, kdey@hmc.edu
Date: 12/7/23

Header for interfacing with QOI compression accelerator
*/

#include <stdint.h> // Include stdint header
#include "debayer.h"

#ifndef qoi_H
#define qoi_H

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

typedef struct {
  char imageData[6100];
  int size;
} encoded_image_Type;

#define QOI_DELAY 100000


///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////


/* Initializes the FPGA for QOI compression
 *    -- reset_pin: pin connected to FPGA rest
 *
 * Note: this also sets up SPI using the pins defined in `STM32L432KC_SPI.h`
 * Refer to our compression protocol explanation (https://kavidey.github.io/NeoObscura/mcu_design/#compression--spi-peripheral) for more details*/ 
void initQOI(int reset_pin);

/* Compresses an image using the QOI algorithm
 *    -- image: pointer to RGBA image to compress
 *    -- encoded_image: pointer to empty buffer to store compressed image in
 *    -- fpga_reset: pin connected to FPGA rest
 *
 * Note: this is a blocking function, it doesn't use any SPI interrupts
 * Refer to our compression protocol explanation (https://kavidey.github.io/NeoObscura/mcu_design/#compression--spi-peripheral) for more details*/ 
void encodeImage(color_pixel_buf_Type *image, encoded_image_Type *encoded_image, int fpga_reset);

#endif