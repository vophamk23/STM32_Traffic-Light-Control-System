/*
 * global.h
 * File này dùng để khai báo các biến toàn cục, enum và hằng số
 * dùng chung cho toàn bộ chương trình điều khiển đèn giao thông.
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

/* ============================================================================
 * CẤU HÌNH TIMER
 * ============================================================================
 *
 * HƯỚNG DẪN:
 * - Chỉ cần thay đổi giá trị của TIMER_INTERRUPT_MS để điều chỉnh chu kỳ ngắt Timer.
 * - Tất cả các hằng số và phép tính phụ thuộc sẽ được tự động cập nhật tương ứng.
 *
 * VÍ DỤ:
 *   TIMER_INTERRUPT_MS = 1   → Ngắt Timer xảy ra mỗi 1ms
 *   TIMER_INTERRUPT_MS = 10  → Ngắt Timer xảy ra mỗi 10ms (mặc định)
 *   TIMER_INTERRUPT_MS = 100 → Ngắt Timer xảy ra mỗi 100ms
 */

// THÔNG SỐ CẤU HÌNH - Chỉ thay đổi giá trị này
#define TIMER_INTERRUPT_MS  10    // Thời gian giữa hai lần ngắt Timer (tính bằng mili giây)

// CÁC HẰNG SỐ TỰ ĐỘNG TÍNH TOÁN (KHÔNG CHỈNH SỬA)
#define CYCLES_PER_SECOND   (1000 / TIMER_INTERRUPT_MS)  // Số lần ngắt xảy ra trong 1 giây
#define CYCLES_PER_250MS    (250 / TIMER_INTERRUPT_MS)   // Số lần ngắt xảy ra trong 250ms

// CÁC HẰNG SỐ DẪN XUẤT PHỤC VỤ CHO HỆ THỐNG
#define TIMER_CYCLE         CYCLES_PER_SECOND    // Dùng để cập nhật đèn giao thông mỗi 1 giây
#define MAX_BLINK_COUNTER   CYCLES_PER_250MS     // Dùng để điều khiển LED nhấp nháy tần số 2Hz (chu kỳ 500ms)


/* ==================================================================
 * KHAI BÁO ENUM (liệt kê các trạng thái, chế độ)
 * ================================================================== */

// Các chế độ hoạt động của hệ thống (MODE)
enum MODE {
    MODE_1_NORMAL = 1,        // Chế độ 1: Hoạt động bình thường (đèn tự chuyển)
    MODE_2_RED_MODIFY = 2,    // Chế độ 2: Chỉnh thời gian đèn đỏ
    MODE_3_AMBER_MODIFY = 3,  // Chế độ 3: Chỉnh thời gian đèn vàng
    MODE_4_GREEN_MODIFY = 4   // Chế độ 4: Chỉnh thời gian đèn xanh
};

// Các trạng thái đèn giao thông (TRAFFIC_STATE)
enum TRAFFIC_STATE {
    INIT,         // Trạng thái khởi tạo ban đầu
    RED_GREEN,    // Đường 1: Đỏ – Đường 2: Xanh
    RED_AMBER,    // Đường 1: Đỏ – Đường 2: Vàng
    GREEN_RED,    // Đường 1: Xanh – Đường 2: Đỏ
    AMBER_RED     // Đường 1: Vàng – Đường 2: Đỏ
};

// Trạng thái nút nhấn (BUTTON_STATE)
enum BUTTON_STATE {
    BTN_RELEASE,  // Nút đang thả ra
    BTN_PRESS     // Nút đang được nhấn
};

/* ==================================================================
 * KHAI BÁO BIẾN TOÀN CỤC (extern: được định nghĩa ở file .c khác)
 * ================================================================== */

// Thời gian hiển thị từng loại đèn (đơn vị: giây)
extern int duration_RED;      // Thời gian đèn đỏ
extern int duration_AMBER;    // Thời gian đèn vàng
extern int duration_GREEN;    // Thời gian đèn xanh

// Biến tạm và bộ đếm thời gian cho từng đường
extern int temp_duration;     // Biến tạm dùng trong chế độ chỉnh thời gian
extern int counter_road1;     // Bộ đếm thời gian cho đường 1
extern int counter_road2;     // Bộ đếm thời gian cho đường 2

// Trạng thái hiện tại của hệ thống
extern enum MODE current_mode;           // Chế độ đang hoạt động
extern enum TRAFFIC_STATE traffic_state; // Trạng thái đèn hiện tại

// Biến phục vụ hiệu ứng nhấp nháy đèn
extern int blink_counter;      // Bộ đếm nhấp nháy
extern int flag_blink;         // Cờ báo hiệu đang nhấp nháy

// Cờ trạng thái của từng loại đèn (cho 2 hướng đường)
extern int flagRed[2];     // Trạng thái đèn đỏ của 2 đường
extern int flagGreen[2];   // Trạng thái đèn xanh của 2 đường
extern int flagYellow[2];  // Trạng thái đèn vàng của 2 đường

// Trạng thái của các nút nhấn
extern enum BUTTON_STATE prevState[3];  // Trạng thái trước đó của 3 nút
extern enum BUTTON_STATE currState[3];  // Trạng thái hiện tại của 3 nút

#endif /* INC_GLOBAL_H_ */
