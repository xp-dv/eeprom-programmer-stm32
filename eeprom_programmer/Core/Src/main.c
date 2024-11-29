/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "circ_buf.h"
#include "uart_rx.h"
#include "print.h" // UART printf() and debugf()
#include <stdint.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum system_state {
  STARTUP_STATE,
  IDLE_STATE,
  READ_STATE,
  WRITE_STATE,
} system_state_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

char printf_buffer[PRINTF_BUF_SIZE] = "";

uart_rx_handle_t uart_rx;
circ_buf_handle_t circ_buf;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//* Private Functions

void print_status(uart_rx_status_t status) {
  printf("[%02d]", status);
}

//* State Event Handlers

system_state_t startup_state_handler(system_state_t system_state) {
  // Startup Delay
  HAL_Delay(500);

  // Startup Message
  uint8_t msg[UART_PACKET_SIZE] = "========== EEPROM PROGRAMMER ==========\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen((char*)msg), HAL_MAX_DELAY);

  // Init Circular Buffer
  circ_buf = circ_buf_init(UART_PACKET_SIZE);

  // Init Packet Buffer
  char delimiter = (char)' ';
  char terminator = (char)'\n';
  uart_rx = uart_rx_init(DATA_PACKET_SIZE, delimiter, terminator);

  // UART Interrupt RX Setup
  HAL_UART_Receive_IT(&huart2, uart_rx_char(uart_rx), 1U);

  // Startup Delay
  HAL_Delay(500);

  return IDLE_STATE;
}

system_state_t idle_state_handler(system_state_t system_state) {
  static uart_rx_status_t status = UART_RX_EMPTY;
  uart_rx_status_t new_status = parse_instruction(uart_rx, circ_buf);
  if (new_status == UART_RX_VALID_PACKET) {
    print_status(new_status);
    switch (uart_rx_instruction(uart_rx)) {
    case READ_INSTRUCTION:
      return READ_STATE;
      break;
    case WRITE_INSTRUCTION:
      return WRITE_STATE;
      break;

    default:
      printf("[Invalid Instruction]\n");
      return IDLE_STATE;
      break;
    }
  } else if ((new_status != UART_RX_EMPTY) && (new_status != status)) {
    print_status(new_status);
    printf("[Invalid Instruction]\n");

    #ifdef UNIT_TEST
    debugf("Packet:\n");
    dump_hex(uart_rx_packet(uart_rx), uart_rx_size(uart_rx), 0x20);
    debugf("Buffer:\n");
    dump_chars(circ_buf_buffer(circ_buf), (circ_buf_size(circ_buf) + 1U), 0x20);
    #endif
  }
  status = new_status;
  HAL_Delay(1000 / PACKET_POLLING_RATE);
  return IDLE_STATE;
}

system_state_t read_state_handler(system_state_t system_state) {
  printf("[Read]\n");
  return IDLE_STATE;
}

system_state_t write_state_handler(system_state_t system_state) {
  printf("[Write]\n");
  return IDLE_STATE;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  //* Next State Variable
  system_state_t system_state = STARTUP_STATE;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    //* State Handlers
    // Perform functions for given state, then return the value of the next state
    switch (system_state) {
    case STARTUP_STATE:
      system_state = startup_state_handler(system_state);
      break;
    case IDLE_STATE:
      system_state = idle_state_handler(system_state);
      break;
    case READ_STATE:
      system_state = read_state_handler(system_state);
      break;
    case WRITE_STATE:
      system_state = write_state_handler(system_state);
      break;

    default:
      system_state = startup_state_handler(system_state);
      break;
    }
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = BAUD_RATE;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SHIFT_CLK_Pin|SHIFT_LATCH_Pin|SHIFT_DATA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(WRITE_ENABLE_GPIO_Port, WRITE_ENABLE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : D7_Pin D8_Pin */
  GPIO_InitStruct.Pin = D7_Pin|D8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : SHIFT_CLK_Pin SHIFT_LATCH_Pin SHIFT_DATA_Pin */
  GPIO_InitStruct.Pin = SHIFT_CLK_Pin|SHIFT_LATCH_Pin|SHIFT_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : D6_Pin D12_Pin D11_Pin D5_Pin */
  GPIO_InitStruct.Pin = D6_Pin|D12_Pin|D11_Pin|D5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : D9_Pin D10_Pin */
  GPIO_InitStruct.Pin = D9_Pin|D10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : WRITE_ENABLE_Pin */
  GPIO_InitStruct.Pin = WRITE_ENABLE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WRITE_ENABLE_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @brief UART Interrupt Callback
 * @param huart HAL UART Structure handle
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  circ_buf_write_ov_byte(circ_buf, *(uart_rx_char(uart_rx)));

  // Reactivate UART Interrupt RX
  HAL_UART_Receive_IT(huart, uart_rx_char(uart_rx), 1U);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
