/*
 * button.c - Xử lý 3 nút nhấn với debounce, short press và long press
 *
 * Cơ chế:
 *   - getKeyInput() gọi mỗi 10ms (trong Task_Button_Scan)
 *   - Debounce: 3 lần đọc liên tiếp giống nhau → xác nhận (30ms)
 *   - Short press: phát hiện cạnh xuống (PRESSED mới)
 *   - Long press: giữ 100 tick × 10ms = 1000ms
 */

#include "button.h"

/* Thanh ghi lịch sử đọc GPIO (4 mẫu cho mỗi nút) */
static int KeyReg0[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
static int KeyReg1[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
static int KeyReg2[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
static int KeyReg3[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};

static int TimeOutForKeyPress[3] = {100, 100, 100};  // 100 × 10ms = 1s

int button_flag[3]         = {0, 0, 0};  // Cờ short press
int button_long_pressed[3] = {0, 0, 0};  // Cờ long press

static int startup_counter = 10;  // Bỏ qua 100ms đầu khi khởi động

/* ============================================================
 * Hàm kiểm tra nút (gọi trong FSM) - tự clear cờ sau khi đọc
 * ============================================================ */
int isButton1Pressed()      { if (button_flag[0])         { button_flag[0] = 0;         return 1; } return 0; }
int isButton2Pressed()      { if (button_flag[1])         { button_flag[1] = 0;         return 1; } return 0; }
int isButton3Pressed()      { if (button_flag[2])         { button_flag[2] = 0;         return 1; } return 0; }
int isButton1LongPressed()  { if (button_long_pressed[0]) { button_long_pressed[0] = 0; return 1; } return 0; }
int isButton2LongPressed()  { if (button_long_pressed[1]) { button_long_pressed[1] = 0; return 1; } return 0; }
int isButton3LongPressed()  { if (button_long_pressed[2]) { button_long_pressed[2] = 0; return 1; } return 0; }

/* ============================================================
 * getKeyInput - Đọc và xử lý nút nhấn, gọi mỗi 10ms
 * ============================================================ */
void getKeyInput(void)
{
    // Bỏ qua 100ms đầu để tránh nhiễu lúc khởi động
    if (startup_counter > 0) { startup_counter--; return; }

    for (int i = 0; i < 3; i++) {
        // Dịch lịch sử đọc
        KeyReg2[i] = KeyReg1[i];
        KeyReg1[i] = KeyReg0[i];

        // Đọc GPIO
        switch (i) {
            case 0: KeyReg0[i] = HAL_GPIO_ReadPin(button1_GPIO_Port, button1_Pin); break;
            case 1: KeyReg0[i] = HAL_GPIO_ReadPin(button2_GPIO_Port, button2_Pin); break;
            case 2: KeyReg0[i] = HAL_GPIO_ReadPin(button3_GPIO_Port, button3_Pin); break;
        }

        // Debounce: 3 mẫu giống nhau
        if (KeyReg0[i] == KeyReg1[i] && KeyReg1[i] == KeyReg2[i]) {

            if (KeyReg3[i] != KeyReg2[i]) {
                // Thay đổi trạng thái → phát hiện short press
                KeyReg3[i] = KeyReg2[i];
                if (KeyReg3[i] == PRESSED_STATE) {
                    button_flag[i] = 1;
                    TimeOutForKeyPress[i] = 100;
                }
            } else {
                // Giữ nguyên → đếm long press
                if (--TimeOutForKeyPress[i] == 0) {
                    TimeOutForKeyPress[i] = 100;
                    if (KeyReg3[i] == PRESSED_STATE) {
                        button_long_pressed[i] = 1;
                    }
                }
            }
        }
    }
}