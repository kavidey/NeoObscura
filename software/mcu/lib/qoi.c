/*
File: qoi.c
Author(s): Neil Chulani, Kavi Dey
Email(s): nchulani@hmc.edu, kdey@hmc.edu
Date: 12/7/23

Source code for interfacing with QOI compression accelerator
*/

#include "STM32L432KC_SPI.h"

#include "qoi.h"
#include "sensor.h"

void initQOI(int reset_pin) {
  pinMode(reset_pin, GPIO_OUTPUT);

  // System Clock: 80 MHz
  // Desired SPI Clock: 5 MHz
  // 80 / 5 = 16 = 2^4

  // polarity = 0, phase = 1
  initSPI(0b111, 0, 1);
}

void encodeImage(color_pixel_buf_Type *image, encoded_image_Type *encoded_image,
                 int fpga_reset) {
  digitalWrite(fpga_reset, 1);

  // Wait for FPGA to reset
  volatile int q = 0;
  while (q < QOI_DELAY) {
    q++;
  }
  spiSendReceive(0xAF);
  spiSendReceive(0xAF);

  digitalWrite(fpga_reset, 0);

  // Send all of the pixels
  for (int w = VERTICAL_RESOLUTION - 1; w >= 0; w--) {
    for (int h = 0; h < HORIZONTAL_RESOLUTION; h++) {
      spiSendReceive((*image)[w][h].r);
      spiSendReceive(0x00);
      spiSendReceive((*image)[w][h].g);
      spiSendReceive(0x00);
      spiSendReceive((*image)[w][h].b);
      spiSendReceive(0x00);
      spiSendReceive((*image)[w][h].a);
      if (!(w == 0 && h == HORIZONTAL_RESOLUTION - 1)) {
        spiSendReceive(0x00);
      }
    }
  }
  // Tell the FPGA that we're done sending pixels
  spiSendReceive(0x08);
  spiSendReceive(0x08);
  spiSendReceive(0x08);

  // Wait for compression to finish
  volatile int delay = 0;
  while (delay < QOI_DELAY) {
    delay++;
  }
  uint8_t recievedByte = 0xBB;
  while (!(recievedByte == 0x80 || recievedByte == 0x81)) {
    recievedByte = spiSendReceive(0xff);
  }

  volatile int j = 14;
  char last8bytes[8] = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
  char encodedBytes[6100];

  // Write QOI Header
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

  // Read until the end of the image
  while ((!((last8bytes[0] == 0x1) && (last8bytes[1] == 0) &&
            (last8bytes[2] == 0) && (last8bytes[3] == 0) &&
            (last8bytes[4] == 0) && (last8bytes[5] == 0) &&
            (last8bytes[6] == 0) && (last8bytes[7] == 0)))) {
    // while (j < 6100){
    encodedBytes[j] = spiSendReceive(0x99);

    // TODO: clean this up with a memcpy
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

  // Patch up the ending; remove the random pixel right before footer, and move
  // the 0x01 one byte earlier Also write 0xCC to the last byte (which used to
  // be 0x01) just for readability so it matches the other intialized values
  encodedBytes[j - 9] = 0x00;
  encodedBytes[j - 2] = 0x01;
  encodedBytes[j - 1] = 0xCC;

  // To help for saving the image
  // Image data will be stored from encodedBytes[0] to
  // encodedBytes[lastByteIndex]
  int lastByteIndex = j - 2;

  for (int x = 0; x <= lastByteIndex; x++) {
    encoded_image->imageData[x] = encodedBytes[x];
  }

  encoded_image->size = lastByteIndex + 1;
}