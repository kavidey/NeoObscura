// STM32L432KC_USB.c
// ADC functions

#include "STM32L432KC_USB.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"
#include "cmsis_gcc.h"
#include "stm32l432xx.h"
#include <stdint.h>

void initUSB() {
  /// Setup USB pins ///
  gpioEnable(GPIO_PORT_A);

  // AF5 for SPI1 on pins PA11 & PA12
  GPIOA->AFR[1] |=
      (0b1010 << GPIO_AFRH_AFSEL11_Pos) | (0b1010 << GPIO_AFRH_AFSEL12_Pos);

  pinMode(PA11, GPIO_ALT);
  pinMode(PA12, GPIO_ALT);

  /// Enable clock to USB ///
  // Enable the 48 MHz clock and wire it to the USB peripheral
  RCC->CRRCR |= RCC_CRRCR_HSI48ON;
  while (!(RCC->CRRCR & RCC_CRRCR_HSI48RDY)) {
  }

  // HSI 48 MHz is selected by default
  // RCC->CCIPR |= _VAL2FLD(RCC_CCIPR_CLK48SEL, 0b00);

  RCC->APB1ENR1 |= RCC_APB1ENR1_USBFSEN | RCC_APB1ENR1_CRSEN;

  /// Turn on analog part of USB transceiver ///
  USB->CNTR |= USB_CNTR_PDWN;

  // wait t_startup (ns)
  // 5 clock cycles should be more than enough
  volatile int x = 100;
  while (x-- > 0)
    __asm("nop");

  /// Enable interrupts///
  // Correct transfer and USB Reset Interrupts
  USB->CNTR |= USB_CNTR_RESETM | USB_CNTR_CTRM;

  // Enable USB Interrupt in NVIC
  NVIC->ISER[2] |= (1 << 3);

  // Enable pullup resistor on D+
  USB->BCDR |= USB_BCDR_DPPU | USB_BCDR_PDEN;

  // Clear pending interrupts
  USB->ISTR = 0;

  // Raise the reset line
  USB->CNTR &= ~(USB_CNTR_FRES);

  USB_ClearSRAM();

  USB_Reset();
}

void USB_ClearSRAM() {
  char *p = (uint8_t *)USB_SRAM;
  for (int i = 0; i < USB_SRAM_SIZE; i++) {
    *++p = 0xe;
  }
  ((uint16_t *) USB_SRAM)[1023] = 0xdead;
}

USB_BTABLE_ENTRY (*BTable)[8] = USB_SRAM;

void USB_EndpointInit() {
  // Reset all endpoint registers
  USB->EP0R = 0;
  USB->EP1R = 0;
  USB->EP2R = 0;
  USB->EP3R = 0;
  USB->EP4R = 0;
  USB->EP5R = 0;
  USB->EP6R = 0;
  USB->EP7R = 0;

  // Setup BTable entry for endpoint 0 (control)
  BTable[0]->ADDR_TX = EP0_TX_BUF_INDEX * 64;
  BTable[0]->COUNT_TX = 0;
  BTable[0]->ADDR_RX = EP0_RX_BUF_INDEX * 64;
  BTable[0]->COUNT_RX =
      USB_COUNTRX_BL1 |
      (1 << USB_COUNT_RX_NUMBLOCK_Pos); // 64 byte buffer size (RM0394 p. 1550)

  // Endpoint type: control, enable receiving, disable transmitting
  USB_SetEPnR(&USB->EP0R, USB_EP_CONTROL | USB_EP_RX_VALID | USB_EP_TX_NAK,
              USB_EP_TYPE_MASK | USB_EP_RX_VALID | USB_EP_TX_VALID);

  // Set address to 0 and enable function
  USB->DADDR = USB_DADDR_EF;
}

void USB_Reset() {
  USB_EndpointInit();

  // Set address to 0 and enable function
  USB->DADDR = USB_DADDR_EF;
}

void USB_IRQHandler() {
  if ((USB->ISTR & USB_ISTR_RESET) != 0) {
    USB_Reset();
    // Clear interrupt
    USB->ISTR = ~USB_ISTR_RESET;
  } else if ((USB->ISTR & USB_ISTR_CTR) != 0) {
    __BKPT();
    // if the message was for the 0 endpoint
    // if ((USB->ISTR & USB_ISTR_EP_ID) == 0) {
    //   USB_HandleControl();
    // }
  }
}

#define EP_rc_w0                                                               \
  0b0111000001110000 // These bits are cleared when writing 0 and remain
                     // unchanged at 1.
#define EP_toggle                                                              \
  0b1000000010000000 // These bits are toggled when writing 1, and remain
                     // unchanged at 0.
#define EP_rw                                                                  \
  0b0000011100001111 // These bits behave "normally", i.e. the written value is
                     // adopted directly.
void USB_SetEPnR(volatile uint16_t *endpoint, uint16_t value, uint16_t mask) {
  uint16_t wr0 = EP_rc_w0 & (~mask | value);
  uint16_t wr1 = (mask & EP_toggle) & (USB->EP0R ^ value);
  uint16_t wr2 = EP_rw & ((USB->EP0R & ~mask) | value);

  USB->EP0R = wr0 | wr1 | wr2;
}

void USB_HandleControl() {
  if (USB->EP0R & USB_EP_CTR_RX) {
    if (USB->EP0R & USB_EP_SETUP) {
      USB_SETUP_PACKET *setup;
      USB_HandleSetup(setup);
    }
    USB_SetEPnR(&USB->EP0R, USB_EP_RX_VALID, USB_EP_RX_VALID);
  }

  if (USB->EP0R & USB_EP_CTR_TX) {
    USB_SetEPnR(&USB->EP0R, USB_EP_TX_DIS, USB_EP_TX_VALID);
  }
}

void USB_HandleSetup(USB_SETUP_PACKET *setup) {
  if ((setup->Request & 0x0F) == 0) { // Device Requests
    switch (setup->RequestType) {
    case 0x01: // Get Status
      __BKPT();
    case 0x06: // Get Descriptor
      USB_BTABLE;
    case 0x08: // Get Configuration
      __BKPT();
    }
  }
}