#ifndef STM32L4_QOI_H
#define STM32L4_QOI_H

#include <stdint.h>
#include <stm32l432xx.h>

#define MSPI                PA6
#define reset               PA5
#define encodedByteReady    PA4
#define readyForPixel       PA3
#define finalPixel          PA1
#define doneEncoding        PA2


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

void initQOI();

struct encodedImage encodeImage(struct pixel* pixels, int numPixels);

#endif