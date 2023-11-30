#include "/Users/neilchulani/Documents/Polarbear/College/Classes/Engineering/MicroPs/HMC-E155/lab/lab07/lab07_mcu/lib/STM32L432KC.h"
#include "/Users/neilchulani/Documents/Polarbear/College/Classes/Engineering/MicroPs/HMC-E155/lab/lab07/lab07_mcu/lib/STM32L432KC_SPI.h"
#include "/Users/neilchulani/Documents/Polarbear/College/Classes/Engineering/MicroPs/HMC-E155/lab/lab07/lab07_mcu/lib/STM32L432KC_GPIO.h"
#include "/Users/neilchulani/Documents/Polarbear/College/Classes/Engineering/MicroPs/HMC-E155/lab/lab07/lab07_mcu/lib/STM32L432KC_RCC.h"
#include "qoi.h"

void initQOI() {

  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
  
  pinMode(reset, GPIO_OUTPUT);  
  pinMode(MSPI, GPIO_OUTPUT); 
  pinMode(finalPixel, GPIO_OUTPUT);
  pinMode(encodedByteReady, GPIO_INPUT); 
  pinMode(readyForPixel, GPIO_INPUT);  
  pinMode(doneEncoding, GPIO_INPUT);
}

struct encodedImage encodeImage(struct pixel* pixels, int numPixels) {
  
  volatile int c = 0;
  digitalWrite(reset, 1); // set reset high
  while (c < 10000) {c++;}
  c = 0;
  digitalWrite(reset, 0); // set reset low

  digitalWrite(MSPI, 0); // Initialize MSPI low
  digitalWrite(finalPixel, 0); // Initialize finalPixel low

  // The max size a 1200 pixel qoi image can be is 1200*5 + 14 + 8 = 6022 bytes
  // Allocate 6100 to be safe
  char encodedBytes[6100]; 

  // Initialize all data to 0xCC so its easy to tell where the image ends in memory view
  for (int count = 0; count < 6100; count++) {
    encodedBytes[count] = 0xCC;
  }

  // start 14 byte header
  encodedBytes[0] = 0x71; // q
  encodedBytes[1] = 0x6F; // o
  encodedBytes[2] = 0x69; // i
  encodedBytes[3] = 0x66; // f

  uint32_t width = 40;
  uint32_t height = 30;

  // 14 byte header
  encodedBytes[4] = ((width >> 24) & 0xFF);
  encodedBytes[5] = ((width >> 16) & 0xFF);
  encodedBytes[6] = ((width >> 8) & 0xFF);
  encodedBytes[7] = (width & 0xFF);
  encodedBytes[8] = ((height >> 24) & 0xFF);
  encodedBytes[9] = ((height >> 16) & 0xFF);
  encodedBytes[10] = ((height >> 8) & 0xFF);
  encodedBytes[11] = (height & 0xFF);
  encodedBytes[12] = 4;
  encodedBytes[13] = 1;
  // end 14 byte header

  // polarity = 0, phase = 1
  initSPI(5, 0, 1);
  
  // start sending/recieving all pixels
  int i = 0;
  volatile int j = 14;
  while (i < numPixels) {
    if (digitalRead(readyForPixel)) {
      if (i == (numPixels - 1)) {
        digitalWrite(finalPixel, 1);
      }
      spiSendReceive(pixels[i].r);
      while(SPI1->SR & SPI_SR_BSY);
      spiSendReceive(pixels[i].g);
      while(SPI1->SR & SPI_SR_BSY);
      spiSendReceive(pixels[i].b);
      while(SPI1->SR & SPI_SR_BSY);
      spiSendReceive(pixels[i].a);
      while(SPI1->SR & SPI_SR_BSY);
      digitalWrite(MSPI, 1);
      while (c < 100) {c++;}
      c = 0;
      digitalWrite(MSPI, 0);
      i++;
    }
    if (digitalRead(encodedByteReady)) {
      encodedBytes[j] = spiSendReceive(0xF);
      while(SPI1->SR & SPI_SR_BSY);
      digitalWrite(MSPI, 1);
      while (c < 100) {c++;}
      c = 0;
      digitalWrite(MSPI, 0);
      j++;
    }
  }

  // This loop is to ensure we get the encoded bytes for the last pixel correctly
  while(!digitalRead(doneEncoding)) {
    if (digitalRead(encodedByteReady)) {
      encodedBytes[j] = spiSendReceive(0xF);
      while(SPI1->SR & SPI_SR_BSY);
      digitalWrite(MSPI, 1);
      while (c < 100) {c++;}
      c = 0;
      digitalWrite(MSPI, 0);
      j++;
    }
  }
  // end sending/recieving all pixels

  // 8 byte end marker
  encodedBytes[j] = 0x00;
  encodedBytes[j+1] = 0x00;
  encodedBytes[j+2] = 0x00;
  encodedBytes[j+3] = 0x00;
  encodedBytes[j+4] = 0x00;
  encodedBytes[j+5] = 0x00;
  encodedBytes[j+6] = 0x00;
  encodedBytes[j+7] = 0x01;

  // To help for saving the image
  // Image data will be stored from encodedBytes[0] to encodedBytes[lastByteIndex]
  int lastByteIndex = j + 7;

  char encodedImageData[lastByteIndex + 1];
  for (int x = 0; x <= lastByteIndex; x++) {
    encodedImageData[x] = encodedBytes[x];
  }

  struct encodedImage result;
  result.imageData = encodedImageData;
  result.size = lastByteIndex + 1;
  return result;
}