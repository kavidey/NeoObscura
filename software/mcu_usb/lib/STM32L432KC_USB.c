// STM32L432KC_USB.c
// ADC functions

#include "STM32L432KC_USB.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"
#include "stm32l432xx.h"

void initUSB() {
  gpioEnable(GPIO_PORT_A);

  /// Enable clock to USB ///
  // Enable the 48 MHz clock and wire it to the USB peripheral
  RCC->CRRCR |= RCC_CRRCR_HSI48ON;

  // HSI 48 MHz is selected by default
  // RCC->CCIPR |= _VAL2FLD(RCC_CCIPR_CLK48SEL, 0b00);

  RCC->APB1ENR1 |= RCC_APB1ENR1_USBFSEN;

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
  USB->BCDR |= USB_BCDR_DPPU;

  // Raise the reset line
  USB->CNTR &= ~(USB_CNTR_FRES);

  // Clear pending interrupts
  // USB->ISTR = 0;
}

void USB_LP_IRQHandler() {
  if ((USB->ISTR & USB_ISTR_RESET) != 0) {
    // Clear interrupt
    USB->ISTR = ~USB_ISTR_RESET;

    // Endpoint type: control, enable receiving, disable transmitting
    USB->EP0R |= USB_EP_CONTROL | USB_EP_RX_VALID | USB_EP_TX_NAK;

    // Set Enable Function
    USB->DADDR |= USB_DADDR_EF;
  } else if ((USB->ISTR & USB_ISTR_CTR) != 0) {
    __BKPT(); // Breakpoint for debugging
  }
}

void USB_HandleControl() {
  
}