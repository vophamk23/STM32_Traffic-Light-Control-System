/*
 * global.c - Định nghĩa các biến toàn cục dùng chung giữa các module
 */

#include "global.h"

/* Thời gian mặc định (giây). Ràng buộc: RED = GREEN + AMBER */
int duration_RED   = 5;
int duration_AMBER = 2;
int duration_GREEN = 3;

/* Giá trị tạm khi đang chỉnh (Mode 2/3/4), lưu vào duration_* khi nhấn SET */
int temp_duration = 0;

/* Bộ đếm ngược hiển thị trên 7-seg (đường 1 = trái, đường 2 = phải) */
int counter_road1 = 0;
int counter_road2 = 0;

/* Trạng thái hệ thống */
enum MODE          current_mode  = MODE_1_NORMAL;
enum TRAFFIC_STATE traffic_state = INIT;

/* Biến điều khiển nhấp nháy LED (chu kỳ 500ms) */
int blink_counter = 0;  // Đếm tick, reset khi đủ MAX_BLINK_COUNTER
int flag_blink    = 0;  // 0 = tắt, 1 = sáng (đảo mỗi 500ms)

/* Cờ trạng thái LED (0=tắt, 1=sáng). Index [0]=Đường 1, [1]=Đường 2 */
int flagRed[2]    = {0, 0};
int flagGreen[2]  = {0, 0};
int flagYellow[2] = {0, 0};

/* Trạng thái nút nhấn cho edge detection (BTN_RELEASE / BTN_PRESS) */
enum BUTTON_STATE prevState[3] = {BTN_RELEASE, BTN_RELEASE, BTN_RELEASE};
enum BUTTON_STATE currState[3] = {BTN_RELEASE, BTN_RELEASE, BTN_RELEASE};