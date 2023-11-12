// STM32F401RE_ADC.c
// ADC functions

#include "STM32L432KC_ADC.h"
#include "STM32L432KC_RCC.h"
#include "stm32l432xx.h"

void initADC(int resolution) {
  // Enable ADC clock
  RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
  RCC->CCIPR |= _VAL2FLD(RCC_CCIPR_ADCSEL, 0b11);

  // Disable Deep Power Down
  ADC1->CR &= ~ADC_CR_DEEPPWD;

  // Enable internal voltage regulator
  ADC1->CR |= ADC_CR_ADVREGEN;

  // Wait until the internal voltage regulator is ready (t_ADCVREG_STUP = 20 ns)
  // 5 clock cycles should be more than enough
  volatile int x = 5;
  while (x-- > 0)
    __asm("nop");

  // Set sampling resolution
  ADC1->CFGR |= resolution;

  // Set left bit alignment
  ADC1->CFGR |= ADC_CFGR_ALIGN;

  // Clear ARDY flag
  ADC1->ISR |= ADC_ISR_ADRDY;

  // Enable ADC
  ADC1->CR |= ADC_CR_ADEN;

  // Wait for ADC to be ready
  while (!(ADC1->ISR & ADC_ISR_ADRDY))
    ;

  // Clear ARDY flag
  ADC1->ISR |= ADC_ISR_ADRDY;
}

void initChannel(int channel) {
  // We only want to do 1 conversion
  ADC1->SQR1 &= ~ADC_SQR1_L;

  // Set channel
  ADC1->SQR1 |= channel << ADC_SQR1_SQ1_Pos;
}

uint16_t readADC() {
  // Start conversion
  ADC1->CR |= ADC_CR_ADSTART;

  // Wait until conversion is done
  while (!(ADC1->ISR & ADC_ISR_EOC))
    ;

  // Return the ADC value
  return ADC1->DR;
}