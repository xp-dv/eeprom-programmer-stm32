#include "uart_rx.h"
#include "circ_buf.h"
#include "eeprom.h"
#include "main.h" // Gives assert_param
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct uart_rx {
  uint8_t* ch; // Store UART interrupt byte/char + NUL
  uint8_t* packet;
  instruction_code_t instruction;
  size_t packet_size;
  uint8_t coded_byte_size; // Size of 1 byte represented in ASCII-Coded hex
  uint8_t coded_address_size; // Chars required to represent EEPROM_ADDRESS_MAX in ASCII-Coded hex
  char delimiter;
  char terminator;
};

//* Private Function Prototypes

uart_rx_status_t uart_rx_strtohex(const uart_rx_handle_t uart_rx, const circ_buf_handle_t circ_buf, size_t* dest, uint8_t coded_size);

static ssize_t strtohex(uint8_t* str);

//* Public Functions

uart_rx_handle_t uart_rx_init(size_t packet_size, char delimiter, char terminator) {
  assert_param(packet_size); // Ensure arguments

  // Allocate dynamic memory for the structure
  uart_rx_handle_t uart_rx = malloc(sizeof(struct uart_rx));
  assert_param(uart_rx); // Ensure allocation successful

  // Allocate dynamic memory to store the incoming packet
  uint8_t* packet = calloc(packet_size, sizeof(uint8_t));
  assert_param(packet); // Ensure allocation successful

  // Initialize members
  uart_rx->ch = calloc(1U, sizeof(uint8_t));
  uart_rx->packet = packet;
  uart_rx->instruction = UART_RX_INVALID_INSTRUCTION;
  uart_rx->packet_size = packet_size;
  uart_rx->coded_byte_size = 2U;
  uart_rx->coded_address_size = 3U;
  uart_rx->delimiter = delimiter;
  uart_rx->terminator = terminator;

  return uart_rx;
}

uart_rx_status_t uart_rx_parse_instruction(const uart_rx_handle_t uart_rx, const circ_buf_handle_t circ_buf) {
  // Check empty
  if ((circ_buf_read_byte(circ_buf, uart_rx->packet) < CIRC_BUF_OK) ||
  (circ_buf_read_byte(circ_buf, &(uart_rx->packet[1])) < CIRC_BUF_OK) ) {
    circ_buf_clear(circ_buf);
    return UART_RX_EMPTY;
  }

  // Check terminator
  if ((char)(uart_rx->packet[1]) != uart_rx->terminator) {
    circ_buf_clear(circ_buf);
    return UART_RX_INVALID_FORMAT;
  }

  uart_rx->instruction = (instruction_code_t)*uart_rx->packet;
  return UART_RX_VALID_PACKET;
}

uart_rx_status_t uart_rx_parse_address(const uart_rx_handle_t uart_rx, const circ_buf_handle_t circ_buf, eeprom_handle_t eeprom, uart_rx_status_t status) {
  if (eeprom->mode == SINGLE_READ_MODE || eeprom->mode == SINGLE_WRITE_MODE) {
    status = uart_rx_strtohex(uart_rx, circ_buf, (size_t*)&eeprom->addresses[0], uart_rx->coded_address_size);
    if (status == UART_RX_VALID_DATA) {
      status = UART_RX_INVALID_FORMAT;
    }
    circ_buf_clear(circ_buf);
    return status;
  } else {
    // Start Address
    status = uart_rx_strtohex(uart_rx, circ_buf, (size_t*)&eeprom->addresses[0], uart_rx->coded_address_size);
    if (status != UART_RX_VALID_DATA) {
      if (status == UART_RX_VALID_PACKET) {
        status = UART_RX_INVALID_FORMAT;
      }
      circ_buf_clear(circ_buf);
      return status;
    }
    // End Address
    status = uart_rx_strtohex(uart_rx, circ_buf, (size_t*)&eeprom->addresses[1], uart_rx->coded_address_size);
    if (status != UART_RX_VALID_PACKET) {
      if (status == UART_RX_VALID_DATA) {
        status = UART_RX_INVALID_FORMAT;
      }
      circ_buf_clear(circ_buf);
      return status;
    }
  }

  return status;
}

uart_rx_status_t uart_rx_parse_data(const uart_rx_handle_t uart_rx, const circ_buf_handle_t circ_buf, uart_rx_status_t status) {
  for (size_t i = 0U; i < uart_rx->packet_size; ++i) {
    status = uart_rx_strtohex(uart_rx, circ_buf, (size_t*)&(uart_rx->packet[i]), uart_rx->coded_byte_size);
    if (status != UART_RX_VALID_DATA) {
      circ_buf_clear(circ_buf);
      return status;
    }
  }

  return status;
}

uart_rx_status_t uart_rx_strtohex(const uart_rx_handle_t uart_rx, const circ_buf_handle_t circ_buf, size_t* dest, uint8_t coded_size) {
  uart_rx_status_t status = UART_RX_INVALID_DATA; // Assume failure

  // Read characters from buffer
  uint8_t* byte = calloc((coded_size + 2U /* delimiter + NUL */), sizeof(uint8_t));
  for (uint8_t i = 0; i < coded_size; ++i) {
    if (circ_buf_read_byte(circ_buf, (&byte[i])) < CIRC_BUF_OK) {
      free(byte);
      return UART_RX_EMPTY;
    }
  }

  // Convert characters to hex
  ssize_t data = strtohex(byte);
  if (data < 0) {
    return UART_RX_INVALID_DATA;
  } else {
    data = data; // Convert to unsigned
  }

  // Check if data is followed by delimiter (next byte) or terminator (end of packet)
  if (circ_buf_read_byte(circ_buf, (&byte[coded_size])) < CIRC_BUF_OK) {
    free(byte);
    return UART_RX_EMPTY;
  }
  if (byte[coded_size] == uart_rx->delimiter) {
    // Data is valid if delimiter character follows the byte
    status = UART_RX_VALID_DATA;
  } else if (byte[coded_size] == uart_rx->terminator) {
    // Both data and packet are valid if terminator character follows the byte
    status = UART_RX_VALID_PACKET;
  } else {
    return UART_RX_INVALID_FORMAT;
  }

  *dest = data; // Save data if both data and packet are valid

  free(byte);
  return status;
}

void uart_rx_clear(const uart_rx_handle_t uart_rx) {
  memset(uart_rx->packet, 0U, uart_rx->packet_size);
}

void uart_rx_free(const uart_rx_handle_t uart_rx) {
  free(uart_rx->packet);
}

uint8_t* uart_rx_char(const uart_rx_handle_t uart_rx) {
  assert_param(uart_rx); // Ensure handle

  return (uart_rx->ch);
}

uint8_t* uart_rx_packet(const uart_rx_handle_t uart_rx) {
  assert_param(uart_rx); // Ensure handle

  return (uart_rx->packet);
}

instruction_code_t uart_rx_instruction(const uart_rx_handle_t uart_rx) {
  assert_param(uart_rx); // Ensure handle

  return (uart_rx->instruction);
}

size_t uart_rx_size(const uart_rx_handle_t uart_rx) {
  assert_param(uart_rx); // Ensure handle

  return (uart_rx->packet_size);
}

//* Private Helper Functions

static ssize_t strtohex(uint8_t* str) {
  ssize_t num = 0;
  if (str != NULL) { // String not empty
    while (*str != (uint8_t)'\0') { // Run till NUL
      // Hex left shift
      num = num << 4U;
      // Add least-significant digit
      if (*str >= '0' && *str <= '9') { // Digits
        num += (*str - '0');
      } else if (*str >= 'A' && *str <= 'F') { // Capital letters
        num += (*str - 'A' + 10U);
      } else if (*str >= 'a' && *str <= 'f') { // Lowercase letters
        num += (*str - 'a' + 10U);
      } else { // Invalid characters
        return -1;
      }
      // Increment to next character
      str++;
    }
  } else { // String empty
    return -1;
  }

  return num;
}
