#pragma once

#include <stdint.h>
#include <stdio.h>

typedef enum status {
  UART_RX_ERR = -1,
  UART_RX_VALID_PACKET,
  UART_RX_VALID_DATA,
} status_t;

struct uart_rx;

typedef struct uart_rx* uart_rx_handle_t;

//* Public Function Prototypes

uart_rx_handle_t uart_rx_init(size_t packet_size, char delimiter, char terminator);
volatile uint8_t* uart_rx_char(const uart_rx_handle_t uart_rx);
