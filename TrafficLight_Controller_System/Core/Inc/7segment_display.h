/*
 * 7seg_display.h
 * File header cho module điều khiển LED 7 đoạn BCD
 *
 * Mô tả hệ thống:
 * - 4 LED 7 đoạn: SEG0, SEG1, SEG2, SEG3
 * - 1 LED 7 đoạn mode: hiển thị chế độ hoạt động (1-4)
 * - Mỗi LED 7 đoạn được điều khiển bằng 4 bit BCD (0-9)
 *
 * Phân bố:
 * - SEG0 + SEG1: Hiển thị bên TRÁI (đường 1 hoặc temp_duration)
 * - SEG2 + SEG3: Hiển thị bên PHẢI (đường 2 hoặc temp_duration)
 * - MODE: Hiển thị số mode (1, 2, 3, 4)
 *
 * Created on: Oct 13, 2025
 * Author: ASUS
 */

#ifndef INC_7SEGMENT_DISPLAY_H_
#define INC_7SEGMENT_DISPLAY_H_

// ============ INCLUDE CÁC THỦ VIỆN CẦN THIẾT ============
#include "main.h"      // Chứa định nghĩa HAL, GPIO pins
#include "global.h"    // Chứa các biến toàn cục (counter, mode, duration...)

/* ==================================================================
 * FUNCTION PROTOTYPES - LED 7 ĐOẠN
 * ================================================================== */

/**
 * @brief Hiển thị số 2 chữ số lên LED 7 đoạn BÊN TRÁI
 *
 * @param num: Số cần hiển thị (0-99)
 *
 * Chi tiết:
 * - Tách số thành hàng chục và hàng đơn vị
 * - SEG0 (PA12-PA15): Hiển thị hàng CHỤC
 * - SEG1 (PB0-PB3): Hiển thị hàng ĐơN VỊ
 * - Sử dụng mã BCD (4 bit cho mỗi chữ số)
 *
 * Ví dụ:
 * - display_7seg_left(45) → SEG0 hiển thị "4", SEG1 hiển thị "5"
 * - display_7seg_left(7) → SEG0 hiển thị "0", SEG1 hiển thị "7"
 *
 * Ứng dụng:
 * - Mode Normal: Hiển thị counter_road1 (thời gian đếm ngược đường 1)
 * - Mode điều chỉnh: Hiển thị temp_duration (giá trị đang chỉnh)
 */
void display_7seg_left(int num);

/**
 * @brief Hiển thị số 2 chữ số lên LED 7 đoạn BÊN PHẢI
 *
 * @param num: Số cần hiển thị (0-99)
 *
 * Chi tiết:
 * - SEG2 (PB4-PB7): Hiển thị hàng CHỤC
 * - SEG3 (PB8-PB11): Hiển thị hàng ĐƠN VỊ
 * - Tương tự display_7seg_left() nhưng dùng các chân GPIO khác
 *
 * Ví dụ:
 * - display_7seg_right(23) → SEG2 hiển thị "2", SEG3 hiển thị "3"
 *
 * Ứng dụng:
 * - Mode Normal: Hiển thị counter_road2 (thời gian đếm ngược đường 2)
 * - Mode điều chỉnh: Hiển thị temp_duration (cùng giá trị với bên trái)
 */
void display_7seg_right(int num);

/**
 * @brief Hiển thị số MODE hiện tại trên LED 7 đoạn MODE
 *
 * @param mode: Số mode cần hiển thị (1-4, hoặc 0-15 với BCD 4-bit)
 *
 * Chi tiết:
 * - Sử dụng 4 chân GPIO PB12-PB15
 * - Hiển thị số mode bằng mã BCD
 * - Giúp người dùng biết hệ thống đang ở chế độ nào
 *
 * Ý nghĩa các mode:
 * - Mode 1: Hoạt động bình thường (automatic traffic light)
 * - Mode 2: Đang điều chỉnh thời gian đèn ĐỎ
 * - Mode 3: Đang điều chỉnh thời gian đèn VÀNG
 * - Mode 4: Đang điều chỉnh thời gian đèn XANH
 *
 * Ví dụ:
 * - display_7seg_mode(1) → Hiển thị "1" (mode normal)
 * - display_7seg_mode(2) → Hiển thị "2" (mode red modify)
 */
void display_7seg_mode(int mode);

/**
 * @brief Hàm chính cập nhật TẤT CẢ LED 7 đoạn theo mode hiện tại
 *
 * @param Không có tham số (đọc từ biến toàn cục)
 *
 * Logic hoạt động:
 *
 * ┌─────────────────────────────────────────────────────────────┐
 * │ if (current_mode == MODE_1_NORMAL)                          │
 * │    ├─ Hiển thị TRÁI: counter_road1 (thời gian đường 1)     │
 * │    ├─ Hiển thị PHẢI: counter_road2 (thời gian đường 2)     │
 * │    └─ Hiển thị MODE: 1                                      │
 * │                                                             │
 * │ else (MODE 2/3/4 - Chế độ điều chỉnh)                      │
 * │    ├─ Hiển thị TRÁI: temp_duration (giá trị đang chỉnh)    │
 * │    ├─ Hiển thị PHẢI: temp_duration (giống bên trái)        │
 * │    └─ Hiển thị MODE: 2, 3, hoặc 4                          │
 * └─────────────────────────────────────────────────────────────┘
 *
 * Cơ chế:
 * - Được gọi trong traffic_run() mỗi 10ms
 * - Tự động chọn dữ liệu hiển thị dựa trên current_mode
 * - Đảm bảo thông tin hiển thị luôn đồng bộ với trạng thái hệ thống
 *
 * Ví dụ hiển thị:
 *
 * MODE 1 (Normal):
 * ┌──────────────────────┐
 * │ [05] [03]      [1]   │  ← Đường 1: 5s, Đường 2: 3s, Mode: 1
 * └──────────────────────┘
 *
 * MODE 2 (Điều chỉnh đèn đỏ):
 * ┌──────────────────────┐
 * │ [08] [08]      [2]   │  ← Đang chỉnh giá trị 8s, Mode: 2
 * └──────────────────────┘
 *
 * Lưu ý:
 * - Hàm này CHỈ CẬP NHẬT hiển thị, không thay đổi giá trị counter
 * - Việc giảm counter được thực hiện trong fsm_normal_mode()
 */
void update_7seg_display(void);

#endif /* INC_7SEGMENT_DISPLAY_H_ */

/* ==================================================================
 * HƯỚNG DẪN SỬ DỤNG MODULE
 * ==================================================================
 *
 * 1. Khởi tạo GPIO pins trong CubeMX:
 *    - PA12-PA15: inputseg0_0 đến inputseg0_3
 *    - PB0-PB3:   inputseg1_0 đến inputseg1_3
 *    - PB4-PB7:   inputseg2_0 đến inputseg2_3
 *    - PB8-PB11:  inputseg3_0 đến inputseg3_3
 *    - PB12-PB15: inputmode_0 đến inputmode_3
 *
 * 2. Trong main.c:
 *    ```c
 *    #include "7segment_display.h"
 *
 *    // Trong timer interrupt mỗi 10ms:
 *    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
 *        if(htim->Instance == TIM2) {
 *            traffic_run();  // Đã bao gồm update_7seg_display()
 *        }
 *    }
 *    ```
 *
 * 3. Kiểm tra hoạt động:
 *    - Quan sát LED 7 đoạn có hiển thị đúng số không
 *    - Kiểm tra số đếm ngược mỗi giây trong Mode 1
 *    - Kiểm tra giá trị temp khi điều chỉnh trong Mode 2/3/4
 *
 * ==================================================================
 */
