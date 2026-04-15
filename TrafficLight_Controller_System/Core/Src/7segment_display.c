/*
 * 7segment_display.c - Hiển thị LED 7 đoạn dạng BCD
 * Mỗi chữ số thập phân → 4 bit BCD xuất ra 4 chân GPIO
 */

#include "7segment_display.h"

/* ============================================================
 * update_7seg_display - Cập nhật toàn bộ 4 LED 7-seg
 * Mode 1 : Hiển thị đếm ngược counter_road1/2 (trái/phải)
 * Mode 2/3/4 : Cả 2 bên hiển thị temp_duration đang chỉnh
 * ============================================================ */
void update_7seg_display(void)
{
    if (current_mode == MODE_1_NORMAL) {
        display_7seg_left(counter_road1);
        display_7seg_right(counter_road2);
        display_7seg_mode(1);
    } else {
        display_7seg_left(temp_duration);
        display_7seg_right(temp_duration);
        display_7seg_mode(current_mode);
    }
}

/* ============================================================
 * display_7seg_left - Hiển thị số (0-99) lên cặp SEG0/SEG1 (bên trái)
 *   SEG0 (hàng chục) : PA12-PA15
 *   SEG1 (hàng đơn vị) : PB0-PB3
 * ============================================================ */
void display_7seg_left(int num)
{
    int tens  = num / 10;
    int units = num % 10;

    // SEG0: hàng chục → PA12-PA15
    HAL_GPIO_WritePin(GPIOA, inputseg0_0_Pin, (tens  & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, inputseg0_1_Pin, (tens  & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, inputseg0_2_Pin, (tens  & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, inputseg0_3_Pin, (tens  & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // SEG1: hàng đơn vị → PB0-PB3
    HAL_GPIO_WritePin(GPIOB, inputseg1_0_Pin, (units & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg1_1_Pin, (units & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg1_2_Pin, (units & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg1_3_Pin, (units & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* ============================================================
 * display_7seg_right - Hiển thị số (0-99) lên cặp SEG2/SEG3 (bên phải)
 *   SEG2 (hàng chục) : PB4-PB7
 *   SEG3 (hàng đơn vị) : PB8-PB11
 * ============================================================ */
void display_7seg_right(int num)
{
    int tens  = num / 10;
    int units = num % 10;

    // SEG2: hàng chục → PB4-PB7
    HAL_GPIO_WritePin(GPIOB, inputseg2_0_Pin, (tens  & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg2_1_Pin, (tens  & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg2_2_Pin, (tens  & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg2_3_Pin, (tens  & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // SEG3: hàng đơn vị → PB8-PB11
    HAL_GPIO_WritePin(GPIOB, inputseg3_0_Pin, (units & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg3_1_Pin, (units & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg3_2_Pin, (units & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg3_3_Pin, (units & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* ============================================================
 * display_7seg_mode - Hiển thị số mode (1-4) lên SEG_MODE
 *   PB12-PB15
 * ============================================================ */
void display_7seg_mode(int mode)
{
    HAL_GPIO_WritePin(GPIOB, inputmode_0_Pin, (mode & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputmode_1_Pin, (mode & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputmode_2_Pin, (mode & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputmode_3_Pin, (mode & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}