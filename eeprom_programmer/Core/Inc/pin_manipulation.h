#pragma once

#include "stm32f3xx_hal.h"
#include <stdint.h>

//* Public Typedefs

typedef enum pin_mode {
  PIN_MODE_OUTPUT = 0,
  PIN_MODE_INPUT,
  PIN_MODE_OUTPUT_HIGH,
  PIN_MODE_INPUT_PULLUP,
  PIN_MODE_INPUT_PULLDOWN,
} pin_mode_t;

typedef enum shift_mode {
  SHIFT_LSB_FIRST,
  SHIFT_MSB_FIRST,
} shift_mode_t;

//* Public Function Prototypes

void shift_word(GPIO_TypeDef* data_port, uint16_t data_pin, GPIO_TypeDef* clock_port, uint16_t clock_pin, shift_mode_t mode, uint16_t word);
uint8_t pin_read(GPIO_TypeDef* port, uint16_t pin);
void pin_write(GPIO_TypeDef* port, uint16_t pin, GPIO_PinState state);
void neg_pulse(GPIO_TypeDef* port, uint16_t pin);
void pin_mode(GPIO_TypeDef* port, uint16_t pin, const pin_mode_t mode);