#ifndef STM32L4_QOI_H
#define STM32L4_QOI_H

#include <stdint.h>
#include <stm32l432xx.h>

#define reset               PA3


struct pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct encodedImage {
  char* imageData;
  int size;
};

void initQOI(void);

struct encodedImage encodeImage(struct pixel* pixels, int numPixels);

#endif