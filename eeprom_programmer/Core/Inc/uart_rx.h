#pragma once

#include "circ_buf.h"
#include <stdint.h>
#include <stdio.h>

typedef enum status {
  UART_RX_INVALID_DELIM = -4,
  UART_RX_INVALID_CHAR,
  UART_RX_READ_FAIL,
  UART_RX_ERR,
  UART_RX_VALID_PACKET,
  UART_RX_VALID_DATA,
} status_t;

struct uart_rx;

typedef struct uart_rx* uart_rx_handle_t;

//* Public Function Prototypes

uart_rx_handle_t uart_rx_init(size_t packet_size, char delimiter, char terminator);

uint8_t* uart_rx_char(const uart_rx_handle_t uart_rx);

uint8_t* uart_rx_packet(const uart_rx_handle_t uart_rx);

size_t uart_rx_size(const uart_rx_handle_t uart_rx);

status_t uart_rx_read_ach(const uart_rx_handle_t uart_rx, const circ_buf_handle_t circ_buf, uint8_t* dest);

status_t uart_rx_read_packet(const uart_rx_handle_t uart_rx, const circ_buf_handle_t circ_buf);

void uart_rx_clear(const uart_rx_handle_t uart_rx);

void uart_rx_free(const uart_rx_handle_t uart_rx);
