/*
 * global.c
 * Module định nghĩa và khởi tạo các biến toàn cục cho hệ thống đèn giao thông
 * Chứa tất cả các biến được chia sẻ giữa các module khác nhau
 */
#include "global.h"

/* ==================================================================
 * BIẾN TOÀN CỤC - DEFINITIONS
 * ================================================================== */

// ============ THỜI GIAN ĐÈN GIAO THÔNG ============
// Các biến này lưu thời gian (tính bằng giây) cho mỗi trạng thái đèn
// Có thể được điều chỉnh thông qua các MODE 2, 3, 4

int duration_RED = 5; // Thời gian đèn ĐỎ sáng: 5 giây
                      // Đèn đỏ = dừng lại, không được đi

int duration_AMBER = 2; // Thời gian đèn VÀNG (AMBER) sáng: 2 giây
                        // Đèn vàng = chuẩn bị dừng hoặc chuyển trạng thái

int duration_GREEN = 3; // Thời gian đèn XANH sáng: 3 giây
                        // Đèn xanh = được phép đi

// LƯU Ý: Công thức đúng cho đèn giao thông:
// duration_RED = duration_GREEN + duration_AMBER
// Ví dụ: RED(5) = GREEN(3) + AMBER(2)

// ============ BIẾN TẠM VÀ COUNTER ============

int temp_duration = 0; // Biến lưu GIÁ TRỊ TẠM THỜI khi điều chỉnh
                       // - Khi ở Mode 2/3/4: lưu giá trị đang chỉnh sửa
                       // - Chỉ cập nhật vào duration_RED/AMBER/GREEN khi nhấn SET

int counter_road1 = 0; // Bộ đếm thời gian đếm ngược cho ĐƯỜNG 1
                       // - Đếm từ duration xuống 0
                       // - Hiển thị trên LED 7 đoạn bên trái

int counter_road2 = 0; // Bộ đếm thời gian đếm ngược cho ĐƯỜNG 2
                       // - Đếm từ duration xuống 0
                       // - Hiển thị trên LED 7 đoạn bên phải

// ============ TRẠNG THÁI HỆ THỐNG ============

enum MODE current_mode = MODE_1_NORMAL;
// Chế độ hoạt động hiện tại của hệ thống
// - MODE_1_NORMAL: Chế độ hoạt động bình thường
// - MODE_2: Chế độ điều chỉnh thời gian đèn ĐỎ
// - MODE_3: Chế độ điều chỉnh thời gian đèn VÀNG
// - MODE_4: Chế độ điều chỉnh thời gian đèn XANH

enum TRAFFIC_STATE traffic_state = INIT;
// Trạng thái đèn giao thông hiện tại
// - INIT: Trạng thái khởi tạo ban đầu
// - RED_GREEN: Đường 1 đỏ, Đường 2 xanh
// - RED_AMBER: Đường 1 đỏ, Đường 2 vàng
// - GREEN_RED: Đường 1 xanh, Đường 2 đỏ
// - AMBER_RED: Đường 1 vàng, Đường 2 đỏ

// ============ BIẾN ĐIỀU KHIỂN LED BLINKING (NHẤP NHÁY) ============

int blink_counter = 0; // Bộ đếm để tạo hiệu ứng nhấp nháy
                       // - Tăng dần mỗi lần timer interrupt (10ms)
                       // - Reset về 0 khi đạt MAX_BLINK_COUNTER

int flag_blink = 0; // Cờ hiệu cho biết trạng thái nhấp nháy
                    // - 0: LED tắt trong chu kỳ nhấp nháy
                    // - 1: LED sáng trong chu kỳ nhấp nháy
                    // - Chuyển đổi giá trị khi blink_counter đạt MAX

// ============ CỜ HIỆU TRẠNG THÁI LED ============
// Các mảng này lưu trạng thái của mỗi loại đèn cho 2 đường
// Index [0] = Đường 1, Index [1] = Đường 2
// Giá trị: 0 = tắt, 1 = sáng

int flagRed[2] = {0, 0}; // Cờ hiệu cho ĐÈN ĐỎ
                         // flagRed[0]: Trạng thái đèn đỏ đường 1
                         // flagRed[1]: Trạng thái đèn đỏ đường 2

int flagGreen[2] = {0, 0}; // Cờ hiệu cho ĐÈN XANH
                           // flagGreen[0]: Trạng thái đèn xanh đường 1
                           // flagGreen[1]: Trạng thái đèn xanh đường 2

int flagYellow[2] = {0, 0}; // Cờ hiệu cho ĐÈN VÀNG
                            // flagYellow[0]: Trạng thái đèn vàng đường 1
                            // flagYellow[1]: Trạng thái đèn vàng đường 2

// ============ TRẠNG THÁI NÚT NHẤN ============
// Lưu trạng thái của 3 nút nhấn: MODE, INC, SET
// Sử dụng để phát hiện sự kiện nhấn nút (edge detection)

enum BUTTON_STATE prevState[3] = {BTN_RELEASE, BTN_RELEASE, BTN_RELEASE};
// Trạng thái TRƯỚC ĐÂY của 3 nút
// - Index [0]: Nút MODE
// - Index [1]: Nút INC (Increase/Tăng)
// - Index [2]: Nút SET
// - BTN_RELEASE: Nút đang được thả ra

enum BUTTON_STATE currState[3] = {BTN_RELEASE, BTN_RELEASE, BTN_RELEASE};
// Trạng thái HIỆN TẠI của 3 nút
// - BTN_RELEASE: Nút không được nhấn
// - BTN_PRESS: Nút đang được nhấn
// - Dùng để so sánh với prevState
//   để phát hiện sự kiện "vừa nhấn"
//   (prevState = RELEASE, currState = PRESS)
