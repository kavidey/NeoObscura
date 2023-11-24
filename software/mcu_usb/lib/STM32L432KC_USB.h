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

#define __USBBUF_BEGIN 0x40006000
#define __MEM2USB(X) (((int)X - __USBBUF_BEGIN) / 2)
#define __USB2MEM(X) (((int)X * 2 + __USBBUF_BEGIN))

typedef struct {
    unsigned short ADDR_TX;
    unsigned short COUNT_TX;
    unsigned short ADDR_RX;
    unsigned short COUNT_RX;
} USB_BTABLE_ENTRY;

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void initUSB();

void USB_HandleControl();
void USB_HandleSetup(USB_SETUP_PACKET *setup);

/*
Modified from https://github.com/CShark/stm32usb/wiki/03.-USB-Reset and
https://www.mikrocontroller.net/articles/USB-Tutorial_mit_STM32
*/
void USB_SetEPnR(volatile uint16_t *endpoint, uint16_t value, uint16_t mask);

#endif