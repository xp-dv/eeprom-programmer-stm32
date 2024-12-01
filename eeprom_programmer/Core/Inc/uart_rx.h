#pragma once

#include "circ_buf.h"
#include <stdint.h>
#include <stdio.h>

typedef enum instruction_code {
  INVALID_INSTRUCTION = 0,
  READ_INSTRUCTION = 'a', // Read Address
  WRITE_INSTRUCTION = 'b', // Write Byte
} instruction_code_t;

typedef enum status {
  UART_RX_EMPTY = -4,
  UART_RX_INVALID_DATA,
  UART_RX_INVALID_FORMAT,
  UART_RX_INVALID_INSTRUCTION,
  UART_RX_VALID_PACKET = 0,
  UART_RX_VALID_DATA = 1,
} uart_rx_status_t;

struct uart_rx;

typedef struct uart_rx* uart_rx_handle_t;

//* Public Function Prototypes

uart_rx_handle_t uart_rx_init(size_t packet_size, char delimiter, char terminator);

uart_rx_status_t uart_rx_parse_instruction(const uart_rx_handle_t uart_rx, const circ_buf_handle_t circ_buf);

uart_rx_status_t uart_rx_parse_address(const uart_rx_handle_t uart_rx, const circ_buf_handle_t circ_buf, eeprom_handle_t eeprom, address_mode_t mode, uart_rx_status_t status);

uart_rx_status_t uart_rx_parse_data(const uart_rx_handle_t uart_rx, const circ_buf_handle_t circ_buf, uart_rx_status_t status);

void uart_rx_clear(const uart_rx_handle_t uart_rx);

void uart_rx_free(const uart_rx_handle_t uart_rx);

uint8_t* uart_rx_char(const uart_rx_handle_t uart_rx);

uint8_t* uart_rx_packet(const uart_rx_handle_t uart_rx);

instruction_code_t uart_rx_instruction(const uart_rx_handle_t uart_rx);

size_t uart_rx_size(const uart_rx_handle_t uart_rx);
