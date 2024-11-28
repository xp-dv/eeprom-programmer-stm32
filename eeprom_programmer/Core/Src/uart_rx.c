#include "uart_rx.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

struct uart_rx {
  volatile uint8_t* ch; // Store UART interrupt byte/char + NUL
  char delimiter;
  char terminator;
  size_t packet_size;
  uint8_t* packet;
};

//* Public Functions

uart_rx_handle_t uart_rx_init(size_t packet_size, char delimiter, char terminator) {
  assert(packet_size); // Ensure arguments

  // Allocate dynamic memory for the structure
  uart_rx_handle_t huart_rx = malloc(sizeof(struct uart_rx));
  assert(huart_rx); // Ensure allocation successful

  // Allocate dynamic memory to store the incoming packet
  uint8_t* packet = calloc(packet_size, sizeof(uint8_t));
  assert(packet); // Ensure allocation successful

  // Initialize members
  huart_rx->ch = calloc(1U, sizeof(uint8_t));
  huart_rx->packet_size = packet_size;
  huart_rx->delimiter = delimiter;
  huart_rx->terminator = terminator;

  return huart_rx;
}

volatile uint8_t* uart_rx_char(const uart_rx_handle_t uart_rx) {
  assert(uart_rx); // Ensure handle

  return (uart_rx->ch);
}
