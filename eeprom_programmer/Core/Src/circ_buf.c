/**
 * @brief Source C file of the circular buffer library.
 * @file circ_buf.c
 * 
 * @author Xavior Pautin (xp.eee116@gmail.com)
 * @date 2024-11-27
 * @copyright Apache-2.0 License
 */
#include "circ_buf.h"
#include "main.h" // Gives assert_param
#include "unit_test.h" // UART printf()
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


/**
 * @struct circ_buf
 * @brief Private circular buffer structure responsible for storing all data
 * necessary for the function of the circular buffer.
 */
struct circ_buf {
  size_t head; // Index of last read byte.
  size_t tail; // Index of last received byte.
  size_t buf_size; // Maximum capacity of data that can be stored in the buffer.
  size_t mem_size; // Actual size of the circular buffer in memory.
  uint8_t* buf; // Pointer to internal buffer.
};

//* Private Function Prototypes

static void increment_tail(const circ_buf_handle_t circ_buf);
static void increment_head(const circ_buf_handle_t circ_buf);

//* Public Functions

circ_buf_handle_t circ_buf_init(size_t buf_size) {
  // Ensure buf_size is within possible range
  assert_param(buf_size > 0 && buf_size < SIZE_MAX);

  // Allocate dynamic memory for the structure
  circ_buf_handle_t circ_buf = malloc(sizeof(struct circ_buf));
  assert_param(circ_buf); // Ensure allocation successful

  // Internally, memory size = buf size + 1 due to how "buffer full" is checked
  circ_buf->buf_size = buf_size;
  circ_buf->mem_size = (buf_size + sizeof(uint8_t));

  // Allocate dynamic memory for the buffer
  uint8_t* buffer = calloc(circ_buf->mem_size, sizeof(uint8_t));
  assert_param(buffer); // Ensure allocation successful
  circ_buf->buf = buffer;

  // Initialize head/tail (read/write) indices
  circ_buf_reset(circ_buf);
  assert_param(circ_buf_is_empty(circ_buf)); // Ensure head and tail initialized to 0

  return circ_buf;
}

void circ_buf_reset(const circ_buf_handle_t circ_buf) {
  assert_param(circ_buf); // Ensure handle

  // Reset elements
  circ_buf->head = 0U;
  circ_buf->tail = 0U;
}

void circ_buf_clear(const circ_buf_handle_t circ_buf) {
  assert_param(circ_buf); // Ensure handle

  // Set memory to 0
  memset(circ_buf, 0U, circ_buf->mem_size);
}

void circ_buf_free(const circ_buf_handle_t circ_buf) {
  assert_param(circ_buf); // Ensure handle

  free(circ_buf); // Release allocated memory
}

uint8_t circ_buf_is_empty(const circ_buf_handle_t circ_buf) {
  uint8_t state = 0; // Assume not empty

  assert_param(circ_buf); // Ensure handle

  state = (circ_buf->tail == circ_buf->head); // Check if buffer is empty

  return state;
}

uint8_t circ_buf_is_full(const circ_buf_handle_t circ_buf) {
  uint8_t state = 0; // Assume not full

  assert_param(circ_buf); // Ensure handle

  // Wrap back to 0 if next index results in the tail overlapping the head
  size_t tail = circ_buf->tail + 1U;
  if (tail == circ_buf->mem_size) { // Faster than index + 1 % mem_size
    tail = 0U;
  }

  state = (tail == circ_buf->head); // Check if buffer is full

  return state;
}

size_t circ_buf_len(const circ_buf_handle_t circ_buf) {
  assert_param(circ_buf); // Ensure handle

  size_t len;
  if (!circ_buf_is_full(circ_buf)) {
    if (circ_buf->head >= circ_buf->tail) {
      len = circ_buf->tail - circ_buf->head;
    } else {
      len = circ_buf->tail + circ_buf->mem_size - circ_buf->head;
    }
  } else {
    len = circ_buf->buf_size;
  }

  return len;
}

size_t circ_buf_size(const circ_buf_handle_t circ_buf) {
  assert_param(circ_buf); // Ensure handle

  return (circ_buf->buf_size);
}

uint8_t* circ_buf_buffer(const circ_buf_handle_t circ_buf) {
  assert_param(circ_buf); // Ensure handle

  return circ_buf->buf;
}

circ_buf_status_t circ_buf_write_byte(const circ_buf_handle_t circ_buf, uint8_t data) {
  circ_buf_status_t status = CIRC_BUF_ERR; // Assume failure

  assert_param(circ_buf); // Ensure handle

  // Don't write byte if buffer is full, otherwise return failure
  if (!circ_buf_is_full(circ_buf)) {
    circ_buf->buf[circ_buf->tail] = data;
    increment_tail(circ_buf);

    status = CIRC_BUF_OK;
  }

  return status;
}

void circ_buf_write_ov_byte(const circ_buf_handle_t circ_buf, uint8_t data) {
  assert_param(circ_buf); // Ensure handle

  circ_buf->buf[circ_buf->tail] = data;

  increment_tail(circ_buf);
}

circ_buf_status_t circ_buf_read_byte(const circ_buf_handle_t circ_buf, uint8_t* dest) {
  circ_buf_status_t status = CIRC_BUF_ERR; // Assume failure

  assert_param(circ_buf && dest); // Ensure handle and destination

  // Don't read byte if buffer is empty, otherwise return failure
  if (!circ_buf_is_empty(circ_buf)) {
    *dest = circ_buf->buf[circ_buf->head];
    increment_head(circ_buf);

    status = CIRC_BUF_OK;
  }

  return status;
}

//* Private Functions

static void increment_tail(const circ_buf_handle_t circ_buf) {
  assert_param(circ_buf); // Ensure handle

  // Reset head index first if buffer is full
  if (circ_buf_is_full(circ_buf)) {
    // Faster than index + 1 % mem_size
    if (++(circ_buf->head) == circ_buf->mem_size) {
      circ_buf->head = 0;
    }
  }

  // Increment tail index
  // Faster than index + 1 % mem_size
  if (++(circ_buf->tail) == circ_buf->mem_size) {
    circ_buf->tail = 0;
  }
}

static void increment_head(const circ_buf_handle_t circ_buf) {
  assert_param(circ_buf); // Ensure handle

  // Increment head index
  // Faster than index + 1 % mem_size
  if (++(circ_buf->head) == circ_buf->mem_size) {
    circ_buf->head = 0;
  }
}

//* Public Testing Functions

#ifdef UNIT_TEST

void dump_indices(const circ_buf_handle_t circ_buf) {
  printf("Head: %u\n", circ_buf->head);
  printf("Tail: %u\n", circ_buf->tail);
}

void dump_hex(const uint8_t* start, size_t size, const size_t columns) {
  // Ensure arguments
  if (!(start && size && columns)) {
    printf("Invalid Args\n");
    return;
  }

  size_t multirow = (columns != SIZE_MAX);

  if (multirow) {
    printf("  0000:");
  }

  for (size_t i = 0; i < size; ++i) {
    // Jump to next row
    if (multirow) {
      if ((i > 0) && (i < size) && (i % columns == 0)) {
        printf("\n  %04X:", i);
      }
    }
    // Print data
    uint8_t c = start[i];
    if (c == 0) {
      printf("  - ");
    } else {
      printf(" x%02X", c % 0x100U);
    }
  }

  printf("\n");
}

void dump_chars(const uint8_t* start, size_t size, const size_t columns) {
  // Ensure arguments
  if (!(start && size && columns)) {
    printf("Invalid Args\n");
    return;
  }

  size_t multirow = (columns != SIZE_MAX);

  if (multirow) {
    printf("  0000:");
  }

  for (size_t i = 0; i < size; ++i) {
    // Jump to next row
    if (multirow) {
      if ((i > 0) && (i < size) && (i % columns == 0)) {
        printf("\n  %04X:", i);
      }
    }
    // Print data
    uint8_t c = start[i];
    if (c < 0x20 || c >= 0x7F) { // 0x20 = SPACE, 0x7F = DEL
      // Handle non-printable characters
      if (c == 0) {
        printf("  - ");
      } else {
        printf(" x%02X", c % 0x100U);
      }
    } else {
      // Handle printable characters
      printf(" '%c'", c);
    }
  }

  printf("\n");
}

#endif /* UNIT_TEST */
