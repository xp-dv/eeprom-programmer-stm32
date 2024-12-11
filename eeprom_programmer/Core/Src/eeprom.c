#include "eeprom.h"
#include "pin_manipulation.h"
#include "circ_buf.h"
#include "main.h"
#include "print.h"

//* Private EEPROM Programming Functions
/**
 * @brief Reads byte from EEPROM at given address.
 * 
 * Pin mode MUST be set to input BEFORE enabling output so that the
 * microcontroller and EEPROM are not both set to OUTPUT at the same time.
 * @param eeprom Pointer to an EEPROM instance.
 * @return uint8_t 
 */
uint8_t read_address(eeprom_handle_t eeprom, uint16_t address);
/**
 * @brief Writes byte to EEPROM at given address.
 * 
 * Output Enable MUST be disabled BEFORE pin mode is set to output so that the
 * microcontroller and EEPROM are not both set to OUTPUT at the same time.
 * @param eeprom Pointer to an EEPROM instance.
 * @param byte Byte to write to EEPROM.
 */
void write_byte(eeprom_handle_t eeprom, uint16_t address, uint8_t byte);

//* Public EEPROM Programming Functions

void set_address(eeprom_handle_t eeprom, uint16_t address) {
  // Mask off bits 15-11
  address &= 0x7FFU;

  // Shift address
  shift_word(eeprom->data_port, eeprom->data_pin, eeprom->clock_port, eeprom->clock_pin, SHIFT_LSB_FIRST, address);

  // Latch data out from the shift register to the storage register which is tied to the output pins
  pin_write(eeprom->latch_port, eeprom->latch_pin, GPIO_PIN_RESET);
  pin_write(eeprom->latch_port, eeprom->latch_pin, GPIO_PIN_SET);
  pin_write(eeprom->latch_port, eeprom->latch_pin, GPIO_PIN_RESET);
}

uint8_t read_address(eeprom_handle_t eeprom, uint16_t address) {
  // Read data bus to byte
  set_address(eeprom, address);
  uint8_t byte = 0U;
  for (int8_t pin = 7; pin >= 0; --pin) {
    byte = (byte << 1U) + pin_read(DATA_BUS_PORT(pin), DATA_BUS_PIN(pin));
  }

  return byte;
}

void write_byte(eeprom_handle_t eeprom, uint16_t address, uint8_t byte) {
  // Write byte to data bus
  set_address(eeprom, address);
  for (uint8_t pin = 0U; pin < 8U; pin++) { // Write data to each arduino pin using LSB mask (data & 1), then shifting out the LSB
    // Mask the LSB
    uint8_t bit = byte & 1U;
    pin_write(DATA_BUS_PORT(pin), DATA_BUS_PIN(pin), bit);
    byte = byte >> 1;
  }

  // Save the data to the EEPROM
  neg_pulse(WRITE_ENABLE_GPIO_Port, WRITE_ENABLE_Pin);
  HAL_Delay(5);
}

void single_read(eeprom_handle_t eeprom) {
  // Set data bus pin mode to input
  for (uint8_t pin = 0; pin < 8; ++pin) {
    pin_mode(DATA_BUS_PORT(pin), DATA_BUS_PIN(pin), PIN_MODE_INPUT);
  }
  // Set Output Enable LOW (Enabled)
  pin_write(OUTPUT_ENABLE_GPIO_Port, OUTPUT_ENABLE_Pin, GPIO_PIN_RESET);

  printf("  %03X: %02X\n", eeprom->addresses[0] % 0x1000, read_address(eeprom, eeprom->addresses[0]));
}

void single_write(eeprom_handle_t eeprom, uint8_t byte) {
  // Set Output Enable HIGH (Disabled)
  pin_write(OUTPUT_ENABLE_GPIO_Port, OUTPUT_ENABLE_Pin, GPIO_PIN_SET);
  // Set data bus pin mode to output
  for (uint8_t pin = 0; pin < 8; pin++) {
    pin_mode(DATA_BUS_PORT(pin), DATA_BUS_PIN(pin), PIN_MODE_OUTPUT);
  }

  write_byte(eeprom, eeprom->addresses[0], byte);
}

void multi_read(eeprom_handle_t eeprom) {
  // Set data bus pin mode to input
  for (uint8_t pin = 0; pin < 8; ++pin) {
    pin_mode(DATA_BUS_PORT(pin), DATA_BUS_PIN(pin), PIN_MODE_INPUT);
  }
  // Set Output Enable LOW (Enabled)
  pin_write(OUTPUT_ENABLE_GPIO_Port, OUTPUT_ENABLE_Pin, GPIO_PIN_RESET);

  uint8_t data_packet[DATA_PACKET_SIZE] = {0};
  uint16_t address = eeprom->addresses[0];
  while (address <= eeprom->addresses[1]) {
    for (uint16_t i = 0U; i < DATA_PACKET_SIZE; ++i) {
      data_packet[i] = read_address(eeprom, address);
      if (address == eeprom->addresses[1]) {
        dump_hex(data_packet, i + 1U, 0x20U);
        return;
      }
      ++address;
    }
    dump_hex(data_packet, DATA_PACKET_SIZE, 0x20U);
  }
}

void multi_write(eeprom_handle_t eeprom, uint8_t* data) {
  // Set Output Enable HIGH (Disabled)
  pin_write(OUTPUT_ENABLE_GPIO_Port, OUTPUT_ENABLE_Pin, GPIO_PIN_SET);
  // Set data bus pin mode to output
  for (uint8_t pin = 0; pin < 8; pin++) {
    pin_mode(DATA_BUS_PORT(pin), DATA_BUS_PIN(pin), PIN_MODE_OUTPUT);
  }

  const uint16_t size = eeprom->addresses[1] - eeprom->addresses[0] + 1U;
  uint16_t address = eeprom->addresses[0];
  for (uint16_t i = 0U; i < size; ++i) {
    write_byte(eeprom, address, data[i]);
    ++address;
  }
}
