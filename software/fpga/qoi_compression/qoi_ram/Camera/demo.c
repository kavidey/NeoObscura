#include <stdio.h>
#include <stm32l432xx.h>
#include "/Users/neilchulani/Documents/Polarbear/College/Classes/Engineering/MicroPs/HMC-E155/lab/lab07/lab07_mcu/lib/STM32L432KC.h"


int main(void) {
    initQOI();

  struct pixel pixels[1200];

  // Begin smiley face 
  for (int b = 0; b < 1200; b++) {
    pixels[b].r = 0x00;
    pixels[b].g = 0x00;
    pixels[b].b = 0xff;
    pixels[b].a = 0xff;
  }

  pixels[293-1].r = 0x00;
  pixels[293-1].g = 0xFF;
  pixels[293-1].b = 0x00;
  pixels[293-1].a = 0xFF;
  pixels[293+15].r = 0x00;
  pixels[293+15].g = 0xFF;
  pixels[293+15].b = 0x00;
  pixels[293+15].a = 0xFF;
  for (int b = 0; b < 15; b++) {
    pixels[733 + b].r = 0x00;
    pixels[733 + b].g = 0xFF;
    pixels[733 + b].b = 0x00;
    pixels[733 + b].a = 0xFF;
  }
  for (int b = 0; b < 4; b++) {
    pixels[573 + (40*b)].r = 0x00;
    pixels[573 + (40*b)].g = 0xFF;
    pixels[573 + (40*b)].b = 0x00;
    pixels[573 + (40*b)].a = 0xFF;
  }
  for (int b = 0; b < 4; b++) {
    pixels[587 + (40*b)].r = 0x00;
    pixels[587 + (40*b)].g = 0xFF;
    pixels[587 + (40*b)].b = 0x00;
    pixels[587 + (40*b)].a = 0xFF;
  }
  // End smiley face
  // This should be a green smiley on a blue background
  
  // run encodeImage on pixels, numPixels
  // pixels should row1, row2, row3 ...
  // A 1D array row by row
  struct encodedImage firstEncodedImage = encodeImage(pixels, 1200);
  // the output will be a struct containing the imageData and the image size
  // the data can be accessed at firstEncodedImage.imageData[0] - firstEncodedImage.imageData[firstEncodedImage.size - 1]
  while(1);
}