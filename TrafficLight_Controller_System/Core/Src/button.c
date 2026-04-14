#include "button.h"

/* ==================================================================
 * GIẢI THÍCH CƠ CHẾ HOẠT ĐỘNG:
 *
 * Module này xử lý 3 nút bấm với các tính năng:
 * 1. DEBOUNCING (Chống dội): Loại bỏ nhiễu cơ học của nút
 * 2. SHORT PRESS: Phát hiện nhấn nhanh
 * 3. LONG PRESS: Phát hiện nhấn giữ (sau 1 giây)
 *
 * THỜI GIAN:
 * - Hàm getKeyInput() được gọi mỗi 10ms trong timer interrupt
 * - Debounce time = 3 lần x 10ms = 30ms
 * - Long press = 100 lần x 10ms = 1000ms (1 giây)
 * ================================================================== */

// ==================================================================
// KHAI BÁO BIẾN TOÀN CỤC
// ==================================================================

// Các thanh ghi lưu trạng thái nút qua các lần đọc (cho 3 nút)
int KeyReg0[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
int KeyReg1[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
int KeyReg2[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
int KeyReg3[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};

// Bộ đếm thời gian cho long press
// 100 lần gọi x 10ms = 1000ms (1 giây)
int TimeOutForKeyPress[3] = {100, 100, 100};

// Cờ báo hiệu nút được nhấn (short press)
int button_flag[3] = {0, 0, 0};

// Cờ báo hiệu nút được nhấn giữ (long press)
int button_long_pressed[3] = {0, 0, 0};

// Bộ đếm khởi động - BỎ QUA NÚT TRONG 100ms ĐẦU
int startup_counter = 10;  // 10 lần x 10ms = 100ms

/* ==================================================================
 * HÀM KIỂM TRA TRẠNG THÁI NÚT (GỌI TRONG MAIN)
 * ================================================================== */

int isButton1Pressed()
{
  if (button_flag[0] == 1)
  {
    button_flag[0] = 0;
    return 1;
  }
  return 0;
}

int isButton2Pressed()
{
  if (button_flag[1] == 1)
  {
    button_flag[1] = 0;
    return 1;
  }
  return 0;
}

int isButton3Pressed()
{
  if (button_flag[2] == 1)
  {
    button_flag[2] = 0;
    return 1;
  }
  return 0;
}

int isButton1LongPressed()
{
  if (button_long_pressed[0] == 1)
  {
    button_long_pressed[0] = 0;
    return 1;
  }
  return 0;
}

int isButton2LongPressed()
{
  if (button_long_pressed[1] == 1)
  {
    button_long_pressed[1] = 0;
    return 1;
  }
  return 0;
}

int isButton3LongPressed()
{
  if (button_long_pressed[2] == 1)
  {
    button_long_pressed[2] = 0;
    return 1;
  }
  return 0;
}

/* ==================================================================
 * XỬ LÝ SỰ KIỆN NÚT BẤM
 * ================================================================== */

void subKeyProcess(int index)
{
  button_flag[index] = 1;
}

/* ==================================================================
 * HÀM CHÍNH - ĐỌC VÀ XỬ LÝ NÚT BẤM (GỌI TRONG TIMER INTERRUPT)
 * ================================================================== */

void getKeyInput()
{
  // BỎ QUA NÚT NHẤN TRONG 100ms ĐẦU TIÊN
  if(startup_counter > 0) {
    startup_counter--;
    return;
  }

  for (int i = 0; i < 3; i++)
  {
    // ===== BƯỚC 1: DỊCH CHUYỂN GIÁ TRỊ CŨ =====
    KeyReg2[i] = KeyReg1[i];
    KeyReg1[i] = KeyReg0[i];

    // ===== BƯỚC 2: ĐỌC GIÁ TRỊ MỚI TỪ GPIO =====
    switch (i)
    {
    case 0:
      KeyReg0[i] = HAL_GPIO_ReadPin(button1_GPIO_Port, button1_Pin);
      break;
    case 1:
      KeyReg0[i] = HAL_GPIO_ReadPin(button2_GPIO_Port, button2_Pin);
      break;
    case 2:
      KeyReg0[i] = HAL_GPIO_ReadPin(button3_GPIO_Port, button3_Pin);
      break;
    }

    // ===== BƯỚC 3: DEBOUNCING - KIỂM TRA 3 LẦN ĐỌC GIỐNG NHAU =====
    if ((KeyReg1[i] == KeyReg0[i]) && (KeyReg1[i] == KeyReg2[i]))
    {
      // ===== BƯỚC 4: PHÁT HIỆN THAY ĐỔI TRẠNG THÁI =====
      if (KeyReg3[i] != KeyReg2[i])
      {
        KeyReg3[i] = KeyReg2[i];

        if (KeyReg3[i] == PRESSED_STATE)
        {
          subKeyProcess(i);
          TimeOutForKeyPress[i] = 100;  // 100 x 10ms = 1000ms
        }
      }
      else
      {
        // ===== BƯỚC 5: XỬ LÝ NHẤN GIỮ (LONG PRESS) =====
        TimeOutForKeyPress[i]--;

        if (TimeOutForKeyPress[i] == 0)
        {
          TimeOutForKeyPress[i] = 100;  // Reset về 1 giây

          if (KeyReg3[i] == PRESSED_STATE)
          {
            button_long_pressed[i] = 1;
          }
        }
      }
    }
  }
}
