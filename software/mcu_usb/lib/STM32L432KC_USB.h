// STM32L432KC_USB.h
// Header for ADC functions

#ifndef STM32L4_USB_H
#define STM32L4_USB_H

#include "stm32l432xx.h"
#include <stdint.h> // Include stdint header

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

typedef struct {
  unsigned short Length;
  unsigned short Index;
  unsigned short Value;
  unsigned char Request;
  unsigned char RequestType;
} USB_SETUP_PACKET;

typedef struct {
  unsigned char Length;
  unsigned char Type;
  unsigned short USBVersion;
  unsigned char DeviceClass;
  unsigned char DeviceSubClass;
  unsigned char DeviceProtocol;
  unsigned char MaxPacketSize;
  unsigned short VendorID;
  unsigned short ProductID;
  unsigned short DeviceVersion;
  unsigned char strManufacturer;
  unsigned char strProduct;
  unsigned char strSerialNumber;
  unsigned char NumConfigurations;
} USB_DESCRIPTOR_DEVICE;

typedef struct {
  unsigned char Length;
  unsigned char DescriptorType;
  unsigned short TotalLength;
  unsigned char NumInterfaces;
  unsigned char ConfigurationValue;
  unsigned char Configuration;
  unsigned char Attributes;
  unsigned char MaxPower;
} USB_DESCRIPTOR_CONFIGURATION;

static const USB_DESCRIPTOR_DEVICE DeviceDescriptor = {.Length = 18,
                                                       .Type = 0x01,
                                                       .USBVersion = 0x0200,
                                                       .DeviceClass = 0x00,
                                                       .DeviceSubClass = 0x00,
                                                       .MaxPacketSize = 64,
                                                       .VendorID = 0x0483,
                                                       .ProductID = 0x5740,
                                                       .DeviceVersion = 0x0001,
                                                       .strManufacturer = 0,
                                                       .strProduct = 0,
                                                       .strSerialNumber = 0,
                                                       .NumConfigurations = 1};
typedef struct {
  volatile uint32_t ADDR_TX;
  volatile uint32_t COUNT_TX;
  volatile uint32_t ADDR_RX;
  volatile uint32_t COUNT_RX;
} USB_BTABLE_ENTRY;

// Modified from: https://github.com/trebisky/stm32f103/blob/master/usb/usb.c
#define USB_SRAM (unsigned long *)0x40006C00
#define USB_SRAM_SIZE 1024 /* bytes */

#define NUM_EP 8

#define EP0_TX_BUF_INDEX	1
#define EP0_RX_BUF_INDEX	2

#define USB_COUNTRX_BL0 (0b0 << 15)
#define USB_COUNTRX_BL1 (0b1 << 15)

#define USB_COUNT_RX_NUMBLOCK_Pos 10
///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void initUSB();

void USB_HandleControl();
void USB_HandleSetup(USB_SETUP_PACKET *setup);

void USB_Reset();

/*
Modified from https://github.com/CShark/stm32usb/wiki/03.-USB-Reset and
https://www.mikrocontroller.net/articles/USB-Tutorial_mit_STM32
*/
void USB_SetEPnR(volatile uint16_t *endpoint, uint16_t value, uint16_t mask);

void USB_EndpointInit();
void USB_ClearSRAM();
#endif