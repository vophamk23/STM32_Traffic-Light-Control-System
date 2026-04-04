/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Traffic Light System - Main Program (FIXED)
  *                   Chương trình chính đi�?u khiển hệ thống đèn giao thông
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"    // Thư viện timer phần m�?m (đếm th�?i gian)
#include "button.h"            // Thư viện xử lý nút nhấn
#include "fsm_traffic.h"       // Thư viện FSM (Finite State Machine) đi�?u khiển đèn giao thông
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Traffic Light System - Main Program (FIXED)
  *                   Chương trình chính điều khiển hệ thống đèn giao thông
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"    // Thư viện timer phần mềm (đếm thời gian)
#include "button.h"            // Thư viện xử lý nút nhấn
#include "fsm_traffic.h"       // Thư viện FSM (Finite State Machine) điều khiển đèn giao thông
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;    // Biến quản lý Timer 2 (phần cứng)

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);       // Hàm cấu hình xung clock hệ thống
static void MX_GPIO_Init(void);      // Hàm khởi tạo GPIO (các chân input/output)
static void MX_TIM2_Init(void);      // Hàm khởi tạo Timer 2

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/**
  * @brief  Timer interrupt callback - Hàm ngắt Timer
  * @param  htim: Timer handle (con trỏ tới timer đang xử lý)
  * @retval None
  *
  * Giải thích:
  * - Hàm này được gọi TỰ ĐỘNG mỗi khi Timer 2 tràn (overflow)
  * - Thời gian gọi phụ thuộc vào cấu hình Timer (thường là 10ms)
  * - Đây là "tim đập" của hệ thống - mọi xử lý theo thời gian đều dựa vào đây
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // Kiểm tra xem ngắt có phải từ Timer 2 không
    if (htim->Instance == TIM2) {

        // 1. Cập nhật bộ đếm timer phần mềm
        //    - Giảm giá trị các timer đã set
        //    - Kiểm tra timer nào hết thời gian
        timerRun();

        // 2. Đọc trạng thái các nút nhấn
        //    - Quét trạng thái nút (nhấn/thả)
        //    - Xử lý chống dội phím (debounce)
        //    - Phát hiện sự kiện nhấn (pressed/long pressed)
        getKeyInput();

        // 3. Chạy máy trạng thái điều khiển đèn giao thông
        //    - Xử lý logic chuyển đèn đỏ-vàng-xanh
        //    - Cập nhật LED 7 đoạn
        //    - Xử lý các mode (normal, adjust red, adjust yellow, adjust green)
        traffic_run();
    }
}

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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  // Khởi tạo hệ thống đèn giao thông
  traffic_init();
  // Bật Timer interrupt
  HAL_TIM_Base_Start_IT(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RED1_Pin|GREEN1_Pin|YELLOW1_Pin|RED2_Pin
                          |GREEN2_Pin|YELLOW2_Pin|inputseg0_0_Pin|inputseg0_1_Pin
                          |inputseg0_2_Pin|inputseg0_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, inputseg1_0_Pin|inputseg1_1_Pin|inputseg1_2_Pin|inputseg3_2_Pin
                          |inputseg3_3_Pin|inputmode_0_Pin|inputmode_1_Pin|inputmode_2_Pin
                          |inputmode_3_Pin|inputseg1_3_Pin|inputseg2_0_Pin|inputseg2_1_Pin
                          |inputseg2_2_Pin|inputseg2_3_Pin|inputseg3_0_Pin|inputseg3_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RED1_Pin GREEN1_Pin YELLOW1_Pin RED2_Pin
                           GREEN2_Pin YELLOW2_Pin inputseg0_0_Pin inputseg0_1_Pin
                           inputseg0_2_Pin inputseg0_3_Pin */
  GPIO_InitStruct.Pin = RED1_Pin|GREEN1_Pin|YELLOW1_Pin|RED2_Pin
                          |GREEN2_Pin|YELLOW2_Pin|inputseg0_0_Pin|inputseg0_1_Pin
                          |inputseg0_2_Pin|inputseg0_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : inputseg1_0_Pin inputseg1_1_Pin inputseg1_2_Pin inputseg3_2_Pin
                           inputseg3_3_Pin inputmode_0_Pin inputmode_1_Pin inputmode_2_Pin
                           inputmode_3_Pin inputseg1_3_Pin inputseg2_0_Pin inputseg2_1_Pin
                           inputseg2_2_Pin inputseg2_3_Pin inputseg3_0_Pin inputseg3_1_Pin */
  GPIO_InitStruct.Pin = inputseg1_0_Pin|inputseg1_1_Pin|inputseg1_2_Pin|inputseg3_2_Pin
                          |inputseg3_3_Pin|inputmode_0_Pin|inputmode_1_Pin|inputmode_2_Pin
                          |inputmode_3_Pin|inputseg1_3_Pin|inputseg2_0_Pin|inputseg2_1_Pin
                          |inputseg2_2_Pin|inputseg2_3_Pin|inputseg3_0_Pin|inputseg3_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : button1_Pin button2_Pin button3_Pin */
  GPIO_InitStruct.Pin = button1_Pin|button2_Pin|button3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */



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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
