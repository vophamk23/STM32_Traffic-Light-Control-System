/*
 * seven_segment.c
 * Module triển khai hiển thị LED 7 đoạn dạng BCD (Binary Coded Decimal)
 * BCD: mỗi chữ số thập phân được mã hóa bằng 4 bit nhị phân
 */
#include "7segment_display.h"


/**
 * Hàm cập nhật toàn bộ màn hình LED 7 đoạn
 *
 * Logic hoạt động:
 * - Kiểm tra mode hiện tại
 * - Hiển thị thông tin tương ứng với từng mode
 */
void update_7seg_display(void)
{
    // ============ CHẾ ĐỘ HOẠT ĐỘNG BÌNH THƯỜNG ============
    if (current_mode == MODE_1_NORMAL)
    {
        // Hiển thị thời gian đếm ngược của đèn giao thông

        display_7seg_left(counter_road1);  // Bên trái: thời gian đường 1
        display_7seg_right(counter_road2); // Bên phải: thời gian đường 2
        display_7seg_mode(1);              // Hiển thị số mode = 1
    }
    // ============ CÁC CHẾ ĐỘ ĐIỀU CHỈNH ============
    else
    {
        // Ở chế độ điều chỉnh (Mode 2, 3, 4):
        // - Cả 2 bên đều hiển thị giá trị đang điều chỉnh
        // - Người dùng có thể thấy rõ giá trị mới đang được thiết lập

        display_7seg_left(temp_duration);  // Bên trái: giá trị tạm thời
        display_7seg_right(temp_duration); // Bên phải: giá trị tạm thời (giống bên trái)
        display_7seg_mode(current_mode);   // Hiển thị số mode hiện tại (2, 3, hoặc 4)
    }
}


/* ==================================================================
 * HIỂN THỊ LED 7 ĐOẠN - ĐỊNH DẠNG BCD
 * ================================================================== */

/**
 * Hàm hiển thị số 2 chữ số lên cặp LED 7 đoạn BÊN TRÁI
 * @param num: Số cần hiển thị (0-99)
 *
 * Cơ chế hoạt động:
 * - Tách số thành hàng chục và hàng đơn vị
 * - Mỗi chữ số được mã hóa thành 4 bit BCD
 * - Xuất 4 bit ra 4 chân GPIO tương ứng
 */
void display_7seg_left(int num)
{
    // Tách số thành 2 chữ số
    int tens = num / 10;  // Lấy chữ số hàng chục (vd: 45 -> 4)
    int units = num % 10; // Lấy chữ số hàng đơn vị (vd: 45 -> 5)

    // ============ SEG0 - LED 7 đoạn hiển thị HÀNG CHỤC ============
    // Sử dụng 4 chân PA12-PA15 của GPIOA

    // Bit 0 (LSB - Least Significant Bit): PA12
    HAL_GPIO_WritePin(GPIOA, inputseg0_0_Pin, (tens & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // Bit 1: PA13
    HAL_GPIO_WritePin(GPIOA, inputseg0_1_Pin, (tens & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // Bit 2: PA14
    HAL_GPIO_WritePin(GPIOA, inputseg0_2_Pin, (tens & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // Bit 3 (MSB - Most Significant Bit): PA15
    HAL_GPIO_WritePin(GPIOA, inputseg0_3_Pin, (tens & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // ============ SEG1 - LED 7 đoạn hiển thị HÀNG ĐƠN VỊ ============
    // Sử dụng 4 chân PB0-PB3 của GPIOB

    // Bit 0: PB0
    HAL_GPIO_WritePin(GPIOB, inputseg1_0_Pin, (units & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // Bit 1: PB1
    HAL_GPIO_WritePin(GPIOB, inputseg1_1_Pin, (units & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // Bit 2: PB2
    HAL_GPIO_WritePin(GPIOB, inputseg1_2_Pin, (units & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // Bit 3: PB3
    HAL_GPIO_WritePin(GPIOB, inputseg1_3_Pin, (units & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * Hàm hiển thị số 2 chữ số lên cặp LED 7 đoạn BÊN PHẢI
 * @param num: Số cần hiển thị (0-99)
 *
 * Tương tự hàm display_7seg_left nhưng dùng các chân GPIO khác
 */
void display_7seg_right(int num)
{
    // Tách số thành 2 chữ số
    int tens = num / 10;  // Hàng chục
    int units = num % 10; // Hàng đơn vị

    // ============ SEG2 - LED 7 đoạn hiển thị HÀNG CHỤC ============
    // Sử dụng 4 chân PB4-PB7 của GPIOB
    HAL_GPIO_WritePin(GPIOB, inputseg2_0_Pin, (tens & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg2_1_Pin, (tens & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg2_2_Pin, (tens & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg2_3_Pin, (tens & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // ============ SEG3 - LED 7 đoạn hiển thị HÀNG ĐƠN VỊ ============
    // Sử dụng 4 chân PB8-PB11 của GPIOB
    HAL_GPIO_WritePin(GPIOB, inputseg3_0_Pin, (units & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg3_1_Pin, (units & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg3_2_Pin, (units & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg3_3_Pin, (units & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * Hàm hiển thị số MODE hiện tại
 * @param mode: Số mode cần hiển thị (0-15, vì dùng 4 bit)
 *
 * Chức năng:
 * - Hiển thị chế độ hoạt động hiện tại của hệ thống
 * - Ví dụ: Mode 1 (Normal), Mode 2 (Adjust Red), Mode 3 (Adjust Yellow), v.v.
 */
void display_7seg_mode(int mode)
{
    // Hiển thị mode sử dụng 4 chân PB12-PB15 của GPIOB

    // Bit 0: PB12
    HAL_GPIO_WritePin(GPIOB, inputmode_0_Pin, (mode & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // Bit 1: PB13
    HAL_GPIO_WritePin(GPIOB, inputmode_1_Pin, (mode & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // Bit 2: PB14
    HAL_GPIO_WritePin(GPIOB, inputmode_2_Pin, (mode & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // Bit 3: PB15
    HAL_GPIO_WritePin(GPIOB, inputmode_3_Pin, (mode & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* ==================================================================
 * KẾT THÚC HIỂN THỊ LED 7 ĐOẠN - ĐỊNH DẠNG BCD
 * ================================================================== */
