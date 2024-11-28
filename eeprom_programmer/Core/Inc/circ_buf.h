/**
 * @brief A circular buffer library that takes a provided buffer and max size,
 * then returns a handle pointer to a circular buffer structure. The library
 * provides several functions to manipulate the buffer through the handle
 * pointer including two types of write/queue.
 * 
 * @file circ_buf.h
 * @author Xavior Pautin (xp.eee116@gmail.com)
 * @date 2024-11-27
 * 
 * @version 0.1
 * @copyright Apache-2.0 License
 */
#pragma once

#include "unit_test.h"
#include <stdlib.h>
#include <stdint.h>

typedef enum circ_buf_status {
  CIRC_BUF_ERR = -1,
  CIRC_BUF_OK,
} circ_buf_status_t;

/**
 * @brief The circular buffer structure is forward declared here to keep it
 * private. An attempt to access this struct will result in an incomplete type
 * error.
 */
struct circ_buf;
/**
 * @brief Public handle that points to the private circular buffer structure.
 */
typedef struct circ_buf* circ_buf_handle_t;

/**
 * @brief Initialize circular buffer structure. The struct is constrained to the
 * library and can only be accessed by the handle returned by the function.
 * Requires a pointer to a user-provided buffer and the value of its max size.
 * The actual memory occupied by the ciruclar buffer will be size + 1 due to its
 * particular implementation.
 * 
 * @param size The maximum size (in bytes) of the circular buffer before data is
 * wrapped back around.
 * @return Pointer to newly initialized circular buffer structure.
 */
circ_buf_handle_t circ_buf_init(size_t size);
/**
 * @brief Resets the head and tail (read/write) indexes.
 * 
 * @param circ_buf Pointer to a circular buffer instance.
 */
void circ_buf_reset(const circ_buf_handle_t circ_buf);
/**
 * @brief Releases the dynamically allocated memory of the circular buffer
 * structure.
 * 
 * @param circ_buf Pointer to a circular buffer instance.
 */
void circ_buf_free(const circ_buf_handle_t circ_buf);
/**
 * @brief Checks if the circular buffer is empty, then returns the result.
 * 
 * @param circ_buf Pointer to a circular buffer instance.
 * @return `true`: 1 if empty, `false`: 0 if not empty.
 */
uint8_t circ_buf_is_empty(const circ_buf_handle_t circ_buf);
/**
 * @brief Checks if the circular buffer is full, then returns the result.
 * 
 * @param circ_buf Pointer to a circular buffer instance.
 * @return `true`: 1 if full, `false`: 0 if not full.
 */
uint8_t circ_buf_is_full(const circ_buf_handle_t circ_buf);
/**
 * @brief Calculates the length of the data currently occupying the
 * circular buffer using its head and tail pointers.
 * 
 * @param circ_buf Pointer to a circular buffer instance.
 * @return Length (in bytes).
 */
size_t circ_buf_len(const circ_buf_handle_t circ_buf);
/**
 * @brief Returns the maximum size the circular buffer can be filled before data
 * is wrapped back around.
 * 
 * @param circ_buf Pointer to a circular buffer instance.
 * @return Size (in bytes).
 */
size_t circ_buf_size(const circ_buf_handle_t circ_buf);
/**
 * @brief Returns a pointer to the internally allocated buffer within the
 * circular buffer struct.
 * 
 * @param circ_buf Pointer to a circular buffer instance.
 * @return Pointer (uint8_t*).
 */
uint8_t* circ_buf_buffer(const circ_buf_handle_t circ_buf);
/**
 * @brief Writes a single byte to the circular buffer, then advances the write
 * index. If the buffer is full, old data still not read/dequeued from the buffer WILL NOT be
 * overwritten.
 * 
 * @param circ_buf Pointer to a circular buffer instance.
 * @param data Byte to write to the circular buffer.
 * @return `CIRC_BUF_OK`: 0, `CIRC_BUF_ERR`: -1
 */
circ_buf_status_t circ_buf_write_byte(const circ_buf_handle_t circ_buf, uint8_t data);
/**
 * @brief (Overwrite) - Writes a single byte to the circular buffer. If the buffer is full,
 * the oldest data still not read/dequeued from the buffer WILL be overwritten.
 * 
 * @param circ_buf Pointer to a circular buffer instance.
 * @param data Byte to write to the circular buffer.
 */
void circ_buf_write_ov_byte(const circ_buf_handle_t circ_buf, uint8_t data);
/**
 * @brief Copies a single byte from the circular buffer
 * 
 * @param circ_buf Pointer to a circular buffer instance.
 * @param dest Destination in memory where the read data should be copied to.
 * @return `CIRC_BUF_OK`: 0, `CIRC_BUF_ERR`: -1
 */
circ_buf_status_t circ_buf_read_byte(const circ_buf_handle_t circ_buf, uint8_t* dest);

#ifdef UNIT_TEST
/**
 * @brief Prints the head and tail indices of the given circular buffer struct.
 * 
 * @param circ_buf Pointer to a circular buffer instance.
 */
void dump_indices(const circ_buf_handle_t circ_buf);
/**
 * @brief Basic memory dump at the given address. All values are printed in hex.
 * 
 * @param start Pointer to the first byte of memory to print.
 * @param size Size in bytes of memory to print.
 * @param columns If columns = SIZE_MAX, the data will print in a single row
 * without any additional formatting.
 */
void dump_hex(const uint8_t* start, size_t size, const size_t columns);
/**
 * @brief Fancy memory dump at the given address. All bytes are printed in hex,
 * except for any ASCII-printable bytes which are printed in char (%c) format
 * surrounded by single quotes.
 * 
 * @param start Pointer to the first byte of memory to print.
 * @param size Size in bytes of memory to print.
 * @param columns If columns = SIZE_MAX, the data will print in a single row
 * without any additional formatting.
 */
void dump_chars(const uint8_t* start, size_t size, const size_t columns);

#endif /* UNIT_TEST */
