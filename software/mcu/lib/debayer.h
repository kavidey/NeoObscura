// debayer.h
// Header for debayer functions

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

// handle out of bounds cases
void edge_handler(uint8_t pixels_in[3][3], lateral_t lateral,
                  vertical_t vertical, uint8_t pixels_out[3][3]);

// The filter3x3 function
rgba_t filter3x3(uint8_t pixels_in[3][3], lateral_t lateral,
                 vertical_t vertical, color_t color);

// The debayer function
void debayer(pixel_buf_Type *input, color_pixel_buf_Type *output);

#endif