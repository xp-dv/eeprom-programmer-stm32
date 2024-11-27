/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BAUD_RATE 115200
#define D7_Pin GPIO_PIN_0
#define D7_GPIO_Port GPIOF
#define D8_Pin GPIO_PIN_1
#define D8_GPIO_Port GPIOF
#define SHIFT_CLK_Pin GPIO_PIN_1
#define SHIFT_CLK_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define SHIFT_LATCH_Pin GPIO_PIN_3
#define SHIFT_LATCH_GPIO_Port GPIOA
#define SHIFT_DATA_Pin GPIO_PIN_4
#define SHIFT_DATA_GPIO_Port GPIOA
#define D6_Pin GPIO_PIN_1
#define D6_GPIO_Port GPIOB
#define D9_Pin GPIO_PIN_8
#define D9_GPIO_Port GPIOA
#define D10_Pin GPIO_PIN_11
#define D10_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA
#define WRITE_ENABLE_Pin GPIO_PIN_3
#define WRITE_ENABLE_GPIO_Port GPIOB
#define D12_Pin GPIO_PIN_4
#define D12_GPIO_Port GPIOB
#define D11_Pin GPIO_PIN_5
#define D11_GPIO_Port GPIOB
#define D5_Pin GPIO_PIN_6
#define D5_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/**
 * ? Pins to SN74HC595 Shift Registers
 * D4 // Connect to SER (Serial)
 * D3 // Connect to RCLK (Storage Register Clock)
 * D1 // Connect to SRCLK (Shift Register Clock)
 */

/**
 * ? Pins to AT28C16 EEPROM
 * D13 // Connect to Write Enable (~WE)
 */

//* Data I/O Bus
#define GET_DATA_BUS_PORT(bit) (\
  (bit) == 0 ? (D5_GPIO_Port) : \
  (bit) == 1 ? (D6_GPIO_Port) : \
  (bit) == 2 ? (D7_GPIO_Port) : \
  (bit) == 3 ? (D8_GPIO_Port) : \
  (bit) == 4 ? (D9_GPIO_Port) : \
  (bit) == 5 ? (D10_GPIO_Port) : \
  (bit) == 6 ? (D11_GPIO_Port) : \
  (bit) == 7 ? (D12_GPIO_Port) : ("0")\
)
#define GET_DATA_BUS_PIN(bit) (\
  (bit) == 0 ? (D5_Pin) : \
  (bit) == 1 ? (D6_Pin) : \
  (bit) == 2 ? (D7_Pin) : \
  (bit) == 3 ? (D8_Pin) : \
  (bit) == 4 ? (D9_Pin) : \
  (bit) == 5 ? (D10_Pin) : \
  (bit) == 6 ? (D11_Pin) : \
  (bit) == 7 ? (D12_Pin) : ("0")\
)

//* Data
#define PACKET_SIZE 256U

//* UART
// Data sent in up to 8 packets of 256 bytes to cover the full 2048 byte memory
#define CIRC_BUF_LEN (PACKET_SIZE + 1U) // data packet + terminator
#define CIRC_BUF_MAX_INDEX (CIRC_BUF_LEN - 1U)
#define UART_TX_BUF_LEN (PACKET_SIZE + 4U) // data packet + status "[XX]"

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */