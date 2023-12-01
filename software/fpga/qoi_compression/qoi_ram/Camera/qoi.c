#include "/Users/neilchulani/Documents/Polarbear/College/Classes/Engineering/MicroPs/HMC-E155/lab/lab07/lab07_mcu/lib/STM32L432KC.h"
#include "/Users/neilchulani/Documents/Polarbear/College/Classes/Engineering/MicroPs/HMC-E155/lab/lab07/lab07_mcu/lib/STM32L432KC_SPI.h"
#include "/Users/neilchulani/Documents/Polarbear/College/Classes/Engineering/MicroPs/HMC-E155/lab/lab07/lab07_mcu/lib/STM32L432KC_GPIO.h"
#include "/Users/neilchulani/Documents/Polarbear/College/Classes/Engineering/MicroPs/HMC-E155/lab/lab07/lab07_mcu/lib/STM32L432KC_RCC.h"
#include "qoi.h"

void initQOI(void) {

  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
  
  pinMode(reset, GPIO_OUTPUT);  
}

struct encodedImage encodeImage(struct pixel* pixels, int numPixels) {
  
  volatile int j = 0;

  // polarity = 0, phase = 1
  initSPI(0, 0, 1);
  digitalWrite(reset, 1);
  volatile int q = 0;
  while (q < 10000) {q++;}
  spiSendReceive(0xAF);
  spiSendReceive(0xAF);
  digitalWrite(reset, 0);
  int i = 0;
  while (i < numPixels) {
    if (!(i == numPixels - 1)) {
      spiSendReceive(pixels[i].r);
      spiSendReceive(0x00);
      spiSendReceive(pixels[i].g);
      spiSendReceive(0x00);
      spiSendReceive(pixels[i].b);
      spiSendReceive(0x00);
      spiSendReceive(pixels[i].a);
      spiSendReceive(0x00);
    }
    if (i == numPixels - 1) {
      spiSendReceive(pixels[i].r);
      spiSendReceive(0x00);
      spiSendReceive(pixels[i].g);
      spiSendReceive(0x00);
      spiSendReceive(pixels[i].b);
      spiSendReceive(0x00);
      spiSendReceive(pixels[i].a);
      spiSendReceive(0x08);
    }
    i++;
  }
  spiSendReceive(0x08);
  spiSendReceive(0x08);
  volatile int delay = 0;
  while (delay < 10000) {delay++;}
  uint8_t recievedByte = 0xBB;
  while (!(recievedByte == 0x80 || recievedByte == 0x81)) {
    recievedByte = spiSendReceive(0xff);
  }
  
  j = 14;
  char last8bytes[8] = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
  char encodedBytes[6100];

  encodedBytes[0] = 0x71; // q
  encodedBytes[1] = 0x6F; // o
  encodedBytes[2] = 0x69; // i
  encodedBytes[3] = 0x66; // f

  uint32_t width = 40;
  uint32_t height = 30;

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

  while ((!((last8bytes[0] == 0x1) && (last8bytes[1] == 0) && (last8bytes[2] == 0) && (last8bytes[3] == 0) && (last8bytes[4] == 0) && (last8bytes[5] == 0) && (last8bytes[6] == 0) && (last8bytes[7] == 0)))) {
  //while (j < 6100){
    encodedBytes[j] = spiSendReceive(0x99);
    last8bytes[7] = last8bytes[6];
    last8bytes[6] = last8bytes[5];
    last8bytes[5] = last8bytes[4];
    last8bytes[4] = last8bytes[3];
    last8bytes[3] = last8bytes[2];
    last8bytes[2] = last8bytes[1];
    last8bytes[1] = last8bytes[0];
    last8bytes[0] = encodedBytes[j];
    j++;
  }
  // Patch up the ending; remove the random pixel right before footer, and move the 0x01 one byte earlier
  // Also write 0xCC to the last byte (which used to be 0x01) just for readability so it matches the other intialized values
  encodedBytes[j-9] = 0x00;
  encodedBytes[j-2] = 0x01;
  encodedBytes[j-1] = 0xCC;

  // To help for saving the image
  // Image data will be stored from encodedBytes[0] to encodedBytes[lastByteIndex]
  int lastByteIndex = j - 2;

  char encodedImageData[lastByteIndex + 1];
  for (int x = 0; x <= lastByteIndex; x++) {
    encodedImageData[x] = encodedBytes[x];
  }

  struct encodedImage result;
  result.imageData = encodedImageData;
  result.size = lastByteIndex + 1;
  return result;
}