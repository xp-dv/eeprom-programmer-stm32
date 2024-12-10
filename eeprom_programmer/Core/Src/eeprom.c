#include "eeprom.h"
#include "pin_manipulation.h"
#include "main.h"
#include "print.h"

//* EEPROM Programming Functions

void set_address(eeprom_handle_t eeprom) {
  // Mask off bits 15-11
  eeprom->addresses[0] &= 0x7FFU;

  // Shift address
  shift_word(eeprom->data_port, eeprom->data_pin, eeprom->clock_port, eeprom->clock_pin, SHIFT_LSB_FIRST, eeprom->addresses[0]);

  // Latch data out from the shift register to the storage register which is tied to the output pins
  pin_write(eeprom->latch_port, eeprom->latch_pin, GPIO_PIN_RESET);
  pin_write(eeprom->latch_port, eeprom->latch_pin, GPIO_PIN_SET);
  pin_write(eeprom->latch_port, eeprom->latch_pin, GPIO_PIN_RESET);
}

uint8_t read_address(eeprom_handle_t eeprom) {
  // Set data bus pin mode to input
  for (uint8_t pin = 0; pin < 8; ++pin) {
    pin_mode(DATA_BUS_PORT(pin), DATA_BUS_PIN(pin), PIN_MODE_INPUT);
  }
  // Set Output Enable LOW (Enable)
  pin_write(OUTPUT_ENABLE_GPIO_Port, OUTPUT_ENABLE_Pin, GPIO_PIN_RESET);

  // Read data bus to byte
  set_address(eeprom);
  uint8_t byte = 0U;
  for (int8_t pin = 7; pin >= 0; --pin) {
    byte = (byte << 1U) + pin_read(DATA_BUS_PORT(pin), DATA_BUS_PIN(pin));
  }

  return byte;
}

void write_byte(eeprom_handle_t eeprom, uint8_t byte) {
  // Set Output Enable LOW (Enable)Q
  pin_write(OUTPUT_ENABLE_GPIO_Port, OUTPUT_ENABLE_Pin, GPIO_PIN_SET);
  // Set data bus pin mode to output
  for (uint8_t pin = 0; pin < 8; pin++) {
    pin_mode(DATA_BUS_PORT(pin), DATA_BUS_PIN(pin), PIN_MODE_OUTPUT);
  }

  // Write byte to data bus
  set_address(eeprom);
  for (uint8_t pin = 0U; pin < 8U; pin++) { // Write data to each arduino pin using LSB mask (data & 1), then shifting out the LSB
    // Mask the LSB
    uint8_t bit = byte & 1U;
    pin_write(DATA_BUS_PORT(pin), DATA_BUS_PIN(pin), bit);
    byte = byte >> 1;
  }

  // Save the data to the EEPROM
  pin_write(WRITE_ENABLE_GPIO_Port, WRITE_ENABLE_Pin, GPIO_PIN_RESET);
  // delayMicroseconds(1);
  pin_write(WRITE_ENABLE_GPIO_Port, WRITE_ENABLE_Pin, GPIO_PIN_SET);
  HAL_Delay(5);
}
