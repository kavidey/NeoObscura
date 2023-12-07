/*
File: debayer.h
Author(s): Henry Merrilees, Kavi Dey
Email(s): hmerrilees@hmc.edu, kdey@hmc.edu
Date: 12/7/23

Header for debayer functions
*/

#include "sensor.h"
#include <stdint.h> // Include stdint header

#ifndef debayer_H
#define debayer_H

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

typedef struct {
  uint8_t r, g, b, a;
} rgba_t;

// NOTE: ROW MAJOR
typedef uint8_t pixel_buf_Type[VERTICAL_RESOLUTION][HORIZONTAL_RESOLUTION];
typedef rgba_t color_pixel_buf_Type[VERTICAL_RESOLUTION][HORIZONTAL_RESOLUTION];

typedef enum { Red, Blue, GreenBesideRed, GreenBesideBlue } color_t;
typedef enum { Left, Center, Right } lateral_t;
typedef enum { Top, Middle, Bottom } vertical_t;

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

/* Handles debayering of pixels on the boundary of the image
 *    -- pixels_in: 3 by 3 array of pixels centered to debayer
 *    -- lateral: what kind of vertical boundary this is (if any)
 *    -- vertical: what kind of horizontal boundary this is (if any)
 *    -- pixels_out: 3 by 3 array of debayered pixels
 * Refer to our debayering explanation (https://kavidey.github.io/NeoObscura/mcu_design/#debayering) for more details*/ 
void edge_handler(uint8_t pixels_in[3][3], lateral_t lateral,
                  vertical_t vertical, uint8_t pixels_out[3][3]);

/* Handles debayering of pixels on the boundary of the image
 *    -- pixels_in: 3 by 3 array of pixels centered to debayer
 *    -- lateral: what kind of vertical boundary this is (if any)
 *    -- vertical: what kind of horizontal boundary this is (if any)
 *    -- color: what color of light the center pixel of was reading
 *
 * returns: RGBA color of center pixel
 * Refer to our debayering explanation (https://kavidey.github.io/NeoObscura/mcu_design/#debayering) for more details*/ 
rgba_t filter3x3(uint8_t pixels_in[3][3], lateral_t lateral,
                 vertical_t vertical, color_t color);

/* Debayers an image
 *    -- input: pointer to input greyscale image
 *    -- output: pointer to output color image (must be same size as input)
 * Refer to our debayering explanation (https://kavidey.github.io/NeoObscura/mcu_design/#debayering) for more details*/ 
void debayer(pixel_buf_Type *input, color_pixel_buf_Type *output);

#endif