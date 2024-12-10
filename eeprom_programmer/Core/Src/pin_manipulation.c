#include "pin_manipulation.h"
#include "stm32f3xx_hal.h"
#include <stdint.h>

void shift_word(GPIO_TypeDef* data_port, uint16_t data_pin, GPIO_TypeDef* clock_port, uint16_t clock_pin, shift_mode_t mode, uint16_t word) {
  const uint8_t WORD_BIT_SIZE = 16U;
  for (uint8_t i = 0U; i < WORD_BIT_SIZE; i++) {
    if (mode == SHIFT_LSB_FIRST) {
      pin_write(data_port, data_pin, !!(word & (1U << i)));
    } else if (mode == SHIFT_MSB_FIRST) {
      pin_write(data_port, data_pin, !!(word & (1U << (WORD_BIT_SIZE - i))));
    }
    pin_write(clock_port, clock_pin, GPIO_PIN_SET);
    pin_write(clock_port, clock_pin, GPIO_PIN_RESET);      
  }
}

uint8_t pin_read(GPIO_TypeDef* port, uint16_t pin) {
  GPIO_PinState pin_state;
  if((port->IDR & pin) != (uint32_t)GPIO_PIN_RESET) {
    pin_state = GPIO_PIN_SET;
  } else {
    pin_state = GPIO_PIN_RESET;
  }

  return pin_state;
}

void pin_write(GPIO_TypeDef* port, uint16_t pin, GPIO_PinState state) {
  if (state != GPIO_PIN_RESET) {
    port->BSRR = (uint32_t)pin;
  } else {
    port->BRR = (uint32_t)pin;
  }
}

void pin_mode(GPIO_TypeDef* port, uint16_t pin, const pin_mode_t mode) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  switch (mode) {
  case PIN_MODE_OUTPUT:
    pin_write(port, pin, GPIO_PIN_RESET); // Set initial state
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    break;
  case PIN_MODE_OUTPUT_HIGH:
    pin_write(port, pin, GPIO_PIN_SET); // Set initial state
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    break;
  case PIN_MODE_INPUT:
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    break;
  case PIN_MODE_INPUT_PULLUP:
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    break;
  case PIN_MODE_INPUT_PULLDOWN:
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    break;
  default:
    break;
  }
  HAL_GPIO_Init(port, &GPIO_InitStruct);
}
