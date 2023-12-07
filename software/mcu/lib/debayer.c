/*
File: debayer.c
Author(s): Henry Merrilees
Email(s): hmerrilees@hmc.edu
Date: 12/7/23

Source code for debayer functions
*/

#include "debayer.h"

void edge_handler(uint8_t pixels_in[3][3], lateral_t lateral,
                  vertical_t vertical, uint8_t pixels_out[3][3]) {
  // Copy the input to output as a baseline
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      pixels_out[i][j] = pixels_in[i][j];
    }
  }

  // Handle lateral cases
  switch (lateral) {
  case Left:
    pixels_out[0][0] = pixels_in[0][2];
    pixels_out[1][0] = pixels_in[1][2];
    pixels_out[2][0] = pixels_in[2][2];
    break;
  case Right:
    pixels_out[0][2] = pixels_in[0][0];
    pixels_out[1][2] = pixels_in[1][0];
    pixels_out[2][2] = pixels_in[2][0];
    break;
  case Center:
    // No changes for the center
    break;
  }

  // Handle vertical cases
  switch (vertical) {
  case Top:
    pixels_out[0][0] = pixels_in[0][0];
    pixels_out[0][1] = pixels_in[0][1];
    pixels_out[0][2] = pixels_in[0][2];
    break;
  case Bottom:
    pixels_out[2][0] = pixels_in[0][0];
    pixels_out[2][1] = pixels_in[0][1];
    pixels_out[2][2] = pixels_in[0][2];
    break;
  case Middle:
    // No changes for the middle
    break;
  }
}

rgba_t filter3x3(uint8_t pixels_in[3][3], lateral_t lateral,
                 vertical_t vertical, color_t color) {
  uint8_t pixels[3][3];
  rgba_t pixel_out;

  edge_handler(pixels_in, lateral, vertical, pixels);

  pixel_out.r = pixel_out.g = pixel_out.b = 0;
  pixel_out.a = 0xff; // opaque

  // Demosaic logic
  switch (color) {
  case Red:
    pixel_out.r = pixels[1][1];
    pixel_out.g =
        (pixels[1][2] + pixels[2][1] + pixels[0][1] + pixels[1][0]) / 4;
    pixel_out.b =
        (pixels[2][2] + pixels[2][0] + pixels[0][2] + pixels[0][0]) / 4;
    break;
  case Blue:
    pixel_out.r =
        (pixels[2][2] + pixels[2][0] + pixels[0][2] + pixels[0][0]) / 4;
    pixel_out.g =
        (pixels[1][2] + pixels[2][1] + pixels[0][1] + pixels[1][0]) / 4;
    pixel_out.b = pixels[1][1];
    break;
  case GreenBesideRed:
    pixel_out.r = (pixels[1][2] + pixels[1][0]) / 2;
    pixel_out.g = pixels[1][1];
    pixel_out.b = (pixels[2][1] + pixels[0][1]) / 2;
    break;
  case GreenBesideBlue:
    pixel_out.r = (pixels[2][1] + pixels[0][1]) / 2;
    pixel_out.g = pixels[1][1];
    pixel_out.b = (pixels[1][2] + pixels[1][0]) / 2;
    break;
  }
  return pixel_out;
}

void debayer(pixel_buf_Type *input, color_pixel_buf_Type *output) {
  rgba_t pixel_out;
  uint8_t pixels_in[3][3];
  uint8_t pixels_out[3][3];

  // current output pixel coordinates
  for (int j = 0; j < VERTICAL_RESOLUTION; ++j) {
    for (int i = 0; i < HORIZONTAL_RESOLUTION; ++i) {

      // Copy the filter input to the 3x3 array
      for (int k = 0; k < 3; ++k) {
        for (int l = 0; l < 3; ++l) {
          int x = i + k - 1;
          int y = j + l - 1;

          pixels_in[l][k] =
              (*input)[y % VERTICAL_RESOLUTION][x % HORIZONTAL_RESOLUTION];
        }
      }

      vertical_t vertical;
      lateral_t lateral;

      switch (i) {
      case (0):
        lateral = Left;
        break;
      case (HORIZONTAL_RESOLUTION - 1):
        lateral = Right;
        break;
      default:
        lateral = Center;
        break;
      }

      switch (j) {
      case (0):
        vertical = Top;
        break;
      case (VERTICAL_RESOLUTION - 1):
        vertical = Bottom;
        break;
      default:
        vertical = Middle;
        break;
      }

      color_t color;

      int x_even = i % 2 == 0;
      int y_even = j % 2 == 0;

      if (x_even && y_even) {
        color = Red;
      } else if (!x_even && y_even) {
        color = GreenBesideRed;
      } else if (x_even && !y_even) {
        color = GreenBesideBlue;
      } else {
        color = Blue;
      }

      // Call the filter
      pixel_out = filter3x3(pixels_in, lateral, vertical, color);
      pixel_out.a = 0xFF; // Set alpha as fully visible
      (*output)[j][i] = pixel_out;
    }
  }
}