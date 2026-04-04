/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Traffic Light System with Cooperative Scheduler
  *                   Hệ thống đèn giao thông sử dụng Scheduler
  ******************************************************************************
  * KIẾN TRÚC MỚI:
  * - Không còn gọi traffic_run() trực tiếp trong timer interrupt
  * - Sử dụng SCH_Add_Task() để thêm các task vào scheduler
  * - Timer interrupt chỉ gọi SCH_Update() (O(1) - rất nhanh!)
  * - Main loop gọi SCH_Dispatch_Tasks() để thực thi task
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "scheduler.h"         // Thư viện Cooperative Scheduler
#include "button.h"            // Thư viện xử lý nút nhấn
#include "fsm_traffic.h"       // Thư viện FSM điều khiển đèn giao thông
#include "tasks.h"        // Tất cả task functions
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;    // Biến quản lý Timer 2


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);


/**
  * ============================================================================
  * TIMER INTERRUPT CALLBACK - CORE CỦA SCHEDULER
  * ============================================================================
  * @brief  Timer interrupt được gọi mỗi 10ms
  * @param  htim: Timer handle
  * @retval None
  *
  * LƯU Ý QUAN TRỌNG:
  * - Hàm này PHẢI giữ ngắn nhất có thể (< 100µs)
  * - CHỈ gọi SCH_Update() - O(1) complexity
  * - KHÔNG thêm bất kỳ code nào khác vào đây!
  * - Mọi xử lý đều phải đưa vào Task
  * ============================================================================
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {
        SCH_Update();
    }
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();


  /* ==================== KHỞI TẠO HỆ THỐNG ==================== */

  // 1. Khởi tạo Scheduler
  SCH_Init();

  // 2. Khởi tạo hệ thống đèn giao thông
  traffic_init();

  /* ========== CORE TASKS - BẮT BUỘC PHẢI CÓ =============== */

   /**
    * Task 1: Button Scanning
    * - Quét nút nhấn mỗi 10ms
    */
   SCH_Add_Task(Task_Button_Scan, 0, TASK_BUTTON_PERIOD);

   /**
    * Task 2: Traffic FSM
    * - Chạy máy trạng thái đèn giao thông mỗi 10ms
    * - Xử lý logic chuyển đèn và mode
    */
   SCH_Add_Task(Task_Traffic_FSM, 0, TASK_FSM_PERIOD);

   /**
    * Task 3: Update Display
    * - Cập nhật LED và 7-segment mỗi 50ms
    * - TÙYCHỈNH: Có thể thay đổi TASK_DISPLAY_PERIOD trong tasks.h
    *   + 20ms: Mượt hơn nhưng tốn CPU
    *   + 50ms: Cân bằng (RECOMMENDED) ✅
    *   + 100ms: Tiết kiệm CPU
    */
   SCH_Add_Task(Task_Update_Display, 0, TASK_DISPLAY_PERIOD);


  // 3. Bật Timer interrupt
  HAL_TIM_Base_Start_IT(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* ==================== DISPATCHER - TIM ĐẬP CHÍNH ==================== */

    /**
     * SCH_Dispatch_Tasks() - Hàm QUAN TRỌNG NHẤT
     *
     * CHỨC NĂNG:
     * - Kiểm tra task nào đã đến giờ chạy (RunMe = 1)
     * - Thực thi các task đó
     * - Cập nhật delay cho các task khác
     *
     * ĐỘ PHỨC TẠP: O(n²) trong trường hợp xấu
     * NHƯNG: Không sao vì chạy trong main loop, không phải interrupt!
     */
    SCH_Dispatch_Tasks();

    // SCH_Go_To_Sleep();

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
