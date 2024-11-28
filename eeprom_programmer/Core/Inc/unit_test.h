#pragma once

#include "main.h"

#ifndef UNIT_TEST
#define UNIT_TEST
#endif

#ifdef UNIT_TEST

#include <stdio.h>

#define UART_TX_TIMEOUT 10
#define PRINTF_BUF_SIZE 1024

extern UART_HandleTypeDef huart2;
extern char print_f[PRINTF_BUF_SIZE];


/**
 * @brief Redirect printf to UART. Note %z format modifier isn't supported.
 */
#define printf(...) \
sprintf(print_f, __VA_ARGS__); \
HAL_UART_Transmit(&huart2, (uint8_t*)print_f, (uint16_t)strlen(print_f), UART_TX_TIMEOUT);

#endif /* UNIT_TEST */
