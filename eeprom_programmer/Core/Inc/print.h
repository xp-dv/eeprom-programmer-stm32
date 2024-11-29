#pragma once

#include "main.h"
#include <stdio.h>

#define PRINTF_TIMEOUT 50
#define PRINTF_BUF_SIZE 1024

extern UART_HandleTypeDef huart2;
extern char printf_buffer[PRINTF_BUF_SIZE];
/**
 * @brief Redirect printf to UART Tx. Use uint8_t instead of char. Note, the %z
 * format specifier isn't supported.
 */
#define printf(...) \
sprintf(printf_buffer, __VA_ARGS__); \
HAL_UART_Transmit(&huart2, (uint8_t*)printf_buffer, (uint16_t)strlen(printf_buffer), PRINTF_TIMEOUT);

//* Debugging

#ifndef UNIT_TEST
#define UNIT_TEST
#endif /* UNIT_TEST */

#ifdef UNIT_TEST
  /**
  * @brief Redirect printf to UART Tx. Note %z format modifier isn't supported.
  */
  #define debugf(...) \
  sprintf(printf_buffer, __VA_ARGS__); \
  HAL_UART_Transmit(&huart2, (uint8_t*)printf_buffer, (uint16_t)strlen(printf_buffer), PRINTF_TIMEOUT);
#else
  #define debugf(...)
#endif /* UNIT_TEST */
