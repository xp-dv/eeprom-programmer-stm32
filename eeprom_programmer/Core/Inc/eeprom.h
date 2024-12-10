#pragma once

#include "main.h"
#include <stdint.h>

//* Public Typedefs

typedef enum rw_mode {
  SINGLE_READ_MODE,
  SINGLE_WRITE_MODE,
  MULTI_READ_MODE,
  MULTI_WRITE_MODE,
} rw_mode_t;

typedef struct eeprom {
  // Pinout
  GPIO_TypeDef* data_port;
  uint16_t data_pin;
  GPIO_TypeDef* clock_port;
  uint16_t clock_pin;
  GPIO_TypeDef* latch_port;
  uint16_t latch_pin;
  // Control
  rw_mode_t mode;
  uint16_t addresses[2];
} eeprom_t;

typedef struct eeprom* eeprom_handle_t;

//* Pin Assignment
/**
 * ? Pins to SN74HC595 Shift Registers
 * D4 // Connect to SER (Serial)
 * D3 // Connect to RCLK (Storage Register Clock)
 * D1 // Connect to SRCLK (Shift Register Clock)
 */
/**
 * ? Pins to AT28C16 EEPROM
 * D13 // Connect to Write Enable (~WE)
 * A4 // Connect to Output Enable (~OE).
 */
#define DATA_BUS_PORT(pin) (\
  (pin) == 0U ? (D5_GPIO_Port) : \
  (pin) == 1U ? (D6_GPIO_Port) : \
  (pin) == 2U ? (D7_GPIO_Port) : \
  (pin) == 3U ? (D8_GPIO_Port) : \
  (pin) == 4U ? (D9_GPIO_Port) : \
  (pin) == 5U ? (D10_GPIO_Port) : \
  (pin) == 6U ? (D11_GPIO_Port) : \
  (pin) == 7U ? (D12_GPIO_Port) : ((GPIO_TypeDef*)0U)\
)
#define DATA_BUS_PIN(pin) (\
  (pin) == 0U ? (D5_Pin) : \
  (pin) == 1U ? (D6_Pin) : \
  (pin) == 2U ? (D7_Pin) : \
  (pin) == 3U ? (D8_Pin) : \
  (pin) == 4U ? (D9_Pin) : \
  (pin) == 5U ? (D10_Pin) : \
  (pin) == 6U ? (D11_Pin) : \
  (pin) == 7U ? (D12_Pin) : ((uint16_t)0U)\
)

//* Public Function Prototypes
/**
 * @brief Reads byte from EEPROM at given address.
 * 
 * Pin mode MUST be set to input BEFORE enabling output so that the
 * microcontroller and EEPROM are not both set to OUTPUT at the same time.
 * @param eeprom Pointer to an EEPROM instance.
 * @return uint8_t 
 */
uint8_t read_address(eeprom_handle_t eeprom);
/**
 * @brief Writes byte to EEPROM at given address.
 * 
 * Output Enable MUST be disabled BEFORE pin mode is set to output so that the
 * microcontroller and EEPROM are not both set to OUTPUT at the same time.
 * @param eeprom Pointer to an EEPROM instance.
 * @param byte Byte to write to EEPROM.
 */
void write_byte(eeprom_handle_t eeprom, uint8_t byte);
/**
 * @brief Shifts the given EEPROM address to the 16-bit address shift register.
 * 
 * - Bits 10-0: The 11-bit target address.
 * 
 * - Bits 15-11: These 5 unused pins are masked off.
 * @param eeprom Pointer to an EEPROM instance.
 */
void set_address(eeprom_handle_t eeprom);
