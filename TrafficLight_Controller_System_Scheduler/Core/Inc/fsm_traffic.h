/*
 * fsm_traffic.h
 * Module máy trạng thái (FSM) điều khiển đèn giao thông
 */

#ifndef INC_FSM_TRAFFIC_H_
#define INC_FSM_TRAFFIC_H_

#include "main.h"
#include "global.h"
#include "button.h"
#include "led_display.h"
#include "7segment_display.h"

/* ==================================================================
 * FUNCTION PROTOTYPES - FSM CONTROL
 * ================================================================== */

/**
 * @brief Khởi tạo hệ thống đèn giao thông
 * @details Thiết lập các giá trị mặc định, reset tất cả biến và LED
 * @note Gọi một lần duy nhất khi khởi động chương trình
 */
void traffic_init(void);

/**
 * @brief Hàm chính của FSM - gọi trong timer interrupt (mỗi 10ms)
 * @details Xử lý logic theo mode hiện tại và cập nhật hiển thị
 * @note Tần suất: 100Hz (100 lần/giây)
 */
void traffic_run(void);

/**
 * @brief Cập nhật trạng thái button (edge detection)
 * @details Đọc trạng thái nút nhấn và phát hiện sự kiện "vừa nhấn"
 */
void update_button_state(void);

/**
 * @brief Tự động điều chỉnh thời gian đèn để đảm bảo ràng buộc RED = GREEN + AMBER
 * @param modified_light Đèn vừa được chỉnh (0=GREEN, 1=AMBER, 2=RED)
 * @return 1 nếu đã điều chỉnh/reset, 0 nếu đã hợp lệ
 * @details
 *   - Nếu chỉnh RED: Giữ AMBER, tính GREEN = RED - AMBER
 *   - Nếu chỉnh AMBER: Giữ GREEN, tính RED = GREEN + AMBER
 *   - Nếu chỉnh GREEN: Giữ AMBER, tính RED = GREEN + AMBER
 *   - Nếu không hợp lệ: Reset về mặc định (RED=5, GREEN=3, AMBER=2)
 */
int auto_adjust_duration(int modified_light);

/**
 * @brief FSM Mode 1: Hoạt động bình thường (đèn giao thông tự động)
 * @details Chu trình: INIT → RED_GREEN → RED_AMBER → GREEN_RED → AMBER_RED → lặp lại
 */
void fsm_normal_mode(void);

/**
 * @brief FSM Mode 2: Điều chỉnh thời gian đèn đỏ
 * @details
 *   - Nút MODE: Chuyển sang Mode 3
 *   - Nút MODIFY: Tăng temp_duration
 *   - Nút SET: Lưu và tự động điều chỉnh 2 đèn còn lại, quay về Mode 1
 *   - LED đỏ nhấp nháy 2Hz
 */
void fsm_red_modify_mode(void);

/**
 * @brief FSM Mode 3: Điều chỉnh thời gian đèn vàng
 * @details
 *   - Nút MODE: Chuyển sang Mode 4
 *   - Nút MODIFY: Tăng temp_duration
 *   - Nút SET: Lưu và tự động điều chỉnh 2 đèn còn lại, quay về Mode 1
 *   - LED vàng nhấp nháy 2Hz
 */
void fsm_amber_modify_mode(void);

/**
 * @brief FSM Mode 4: Điều chỉnh thời gian đèn xanh
 * @details
 *   - Nút MODE: Quay về Mode 1
 *   - Nút MODIFY: Tăng temp_duration
 *   - Nút SET: Lưu và tự động điều chỉnh 2 đèn còn lại, quay về Mode 1
 *   - LED xanh nhấp nháy 2Hz
 */
void fsm_green_modify_mode(void);

#endif /* INC_FSM_TRAFFIC_H_ */
