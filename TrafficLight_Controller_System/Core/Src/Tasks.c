/*
 * ============================================================================
 * tasks.c - TASK FUNCTIONS FOR SCHEDULER
 * ============================================================================
 *
 */

#include "tasks.h"

/* ============================================================================
 * TASK 1: BUTTON SCANNING
 * ============================================================================
 * Mục đích: Quét và xử lý trạng thái nút nhấn
 * Tần suất: 10ms (PERIOD = 1)
 * Độ ưu tiên: CAO - Phải chạy nhanh để phát hiện nhấn chính xác
 * ============================================================================ */

/**
 * Task_Button_Scan - Quét nút nhấn mỗi 10ms
 *
 * Chức năng:
 * - Đọc trạng thái GPIO của 3 nút (MODE, MODIFY, SET)
 * - Xử lý debouncing (chống dội)
 * - Phát hiện sự kiện short press và long press
 * - Cập nhật các cờ button_flag[] và button_long_pressed[]
 *
 * Lưu ý:
 * - Task này PHẢI chạy mỗi 10ms (không được chậm hơn)
 * - Nếu chạy chậm → nút nhấn sẽ bị miss
 */
void Task_Button_Scan(void)
{
    getKeyInput();  // Hàm từ button.c - xử lý debounce và edge detection
}

/* ============================================================================
 * TASK 2: TRAFFIC LIGHT FSM
 * ============================================================================
 * Mục đích: Điều khiển máy trạng thái đèn giao thông
 * Tần suất: 10ms (PERIOD = 1)
 * Độ ưu tiên: CAO - Cốt lõi của hệ thống
 * ============================================================================ */

/**
 * Task_Traffic_FSM - Chạy FSM đèn giao thông
 *
 * Chức năng:
 * - Cập nhật trạng thái nút nhấn (edge detection)
 * - Xử lý FSM theo mode hiện tại:
 *   + MODE_1: Hoạt động tự động (RED_GREEN → RED_AMBER → GREEN_RED → AMBER_RED)
 *   + MODE_2: Điều chỉnh thời gian đèn ĐỎ (nhấp nháy đèn đỏ)
 *   + MODE_3: Điều chỉnh thời gian đèn VÀNG (nhấp nháy đèn vàng)
 *   + MODE_4: Điều chỉnh thời gian đèn XANH (nhấp nháy đèn xanh)
 * - Cập nhật bộ đếm thời gian (counter_road1, counter_road2)
 *
 * Lưu ý:
 * - Task này chứa toàn bộ logic điều khiển
 * - Không tự cập nhật LED/7-segment
 */
void Task_Traffic_FSM(void)
{
    traffic_run();  // Hàm từ fsm_traffic.c - xử lý toàn bộ FSM
}

/* ============================================================================
 * TASK 3: UPDATE DISPLAY
 * ============================================================================
 * Mục đích: Cập nhật hiển thị LED và LED 7 đoạn
 * Tần suất: 50ms (PERIOD = 5) - Có thể điều chỉnh
 * Độ ưu tiên: TRUNG BÌNH - Không cần cập nhật quá nhanh
 * ============================================================================ */

/**
 * Task_Update_Display - Cập nhật phần cứng hiển thị
 *
 * Chức năng:
 * - Cập nhật 6 LED đèn giao thông (đỏ, vàng, xanh x 2 đường)
 * - Cập nhật 4 LED 7 đoạn (hiển thị thời gian đếm ngược)
 * - Cập nhật LED mode (hiển thị chế độ hiện tại)
 *
 * Tối ưu hóa:
 * - PERIOD = 5 (50ms): Cân bằng giữa mượt mà và hiệu năng ✅ RECOMMENDED
 * - PERIOD = 2 (20ms): Mượt hơn, tốn CPU hơn
 * - PERIOD = 10 (100ms): Tiết kiệm CPU, nhưng có thể thấy giật
 *
 * Lưu ý:
 * - Task này chỉ HIỂN THỊ, không xử lý logic
 * - Logic đã được xử lý trong Task_Traffic_FSM
 */
void Task_Update_Display(void)
{
    // Cập nhật LED đơn (đèn giao thông)
    update_led_display();    // Hàm từ led_display.c

    // Cập nhật LED 7 đoạn (đồng hồ đếm ngược)
    update_7seg_display();   // Hàm từ 7segment_display.c
}
