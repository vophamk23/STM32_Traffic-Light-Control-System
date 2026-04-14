/*
 * ============================================================================
 * fsm_traffic.c - HỆ THỐNG ĐIỀU KHIỂN ĐÈN GIAO THÔNG
 * ============================================================================
 *
 * TỔNG QUAN:
 * Bộ điều khiển đèn giao thông cho giao lộ 2 chiều với 4 chế độ:
 *
 * CHẾ ĐỘ 1: Hoạt động tự động (chuyển đèn tự động)
 * CHẾ ĐỘ 2: Điều chỉnh thời gian ĐỎ
 * CHẾ ĐỘ 3: Điều chỉnh thời gian VÀNG
 * CHẾ ĐỘ 4: Điều chỉnh thời gian XANH
 *
 * RÀNG BUỘC: thời_gian_ĐỎ = thời_gian_XANH + thời_gian_VÀNG
 *
 * PHẦN CỨNG:
 * - Nút 1 (MODE): Chuyển đổi giữa các chế độ
 * - Nút 2 (MODIFY/INC): Tăng giá trị thời gian
 * - Nút 3 (SET): Lưu giá trị và quay về chế độ tự động
 * - LED: 6 LED cho đèn giao thông (3 màu x 2 đường)
 * - LED 7 đoạn: Hiển thị đồng hồ đếm ngược
 *
 * TỐC ĐỘ CẬP NHẬT: 10ms (100Hz)
 * ============================================================================
 */

#include "fsm_traffic.h"

/* ============================================================================
 * KHỞI TẠO HỆ THỐNG
 * ============================================================================ */

/**
 * traffic_init() - Khởi tạo toàn bộ hệ thống giao thông
 *
 * Đặt thời gian mặc định, chế độ, trạng thái và trạng thái nút nhấn
 * Được gọi một lần trong main() trước khi vào vòng lặp chính
 */
void traffic_init(void)
{
	// Thời gian mặc định
	duration_RED = 5;      // Đèn đỏ: 5 giây
	duration_AMBER = 2;    // Đèn vàng: 2 giây
	duration_GREEN = 3;    // Đèn xanh: 3 giây

	// Chế độ và trạng thái ban đầu
    current_mode = MODE_1_NORMAL;
    traffic_state = INIT;  // Sẽ chuyển sang RED_GREEN khi chạy lần đầu

    // Khởi tạo bộ đếm
    counter_road1 = 0;
    counter_road2 = 0;

    // Tắt tất cả LED
    turn_off_all_leds();

    // Khởi tạo phát hiện cạnh nút nhấn
    prevState[0] = BTN_RELEASE;     // Nút MODE
    prevState[1] = BTN_RELEASE;     // Nút MODIFY
    prevState[2] = BTN_RELEASE;     // Nút SET

    currState[0] = BTN_RELEASE;
    currState[1] = BTN_RELEASE;
    currState[2] = BTN_RELEASE;

    // Khởi tạo nhấp nháy LED
    // Chu kỳ: 0.25s BẬT + 0.25s TẮT = 0.5s
    blink_counter = 0;
    flag_blink = 0;

    // Reset cờ LED
    flagRed[0] = flagRed[1] = 0;
    flagGreen[0] = flagGreen[1] = 0;
    flagYellow[0] = flagYellow[1] = 0;

    // Reset biến tạm
    temp_duration = 0;
}

/* ============================================================================
 * HÀM CHÍNH - TIM ĐẬP CỦA HỆ THỐNG
 * ============================================================================ */

/**
 * traffic_run() - Hàm chính được gọi mỗi 10ms
 *
 * THỨ TỰ THỰC THI (QUAN TRỌNG):
 * 1. update_button_state()   → Đọc trạng thái nút nhấn
 * 2. fsm_*_mode()            → Xử lý logic theo chế độ hiện tại
 * 3. update_led_display()    → Cập nhật phần cứng LED
 * 4. update_7seg_display()   → Cập nhật màn hình 7 đoạn
 *
 * TỐC ĐỘ GỌI: 100 lần/giây = 100Hz
 */
void traffic_run(void)
{
	// Bước 1: Đọc trạng thái nút nhấn
	update_button_state();

    // Bước 2: Xử lý logic theo chế độ
    switch(current_mode) {
        case MODE_1_NORMAL:
            fsm_normal_mode();    // Chế độ tự động
            break;

        case MODE_2_RED_MODIFY:
            fsm_red_modify_mode();    // Điều chỉnh thời gian ĐỎ
            break;

        case MODE_3_AMBER_MODIFY:
            fsm_amber_modify_mode();  // Điều chỉnh thời gian VÀNG
            break;

        case MODE_4_GREEN_MODIFY:
            fsm_green_modify_mode();  // Điều chỉnh thời gian XANH
            break;
    }

    // Bước 3: Cập nhật phần cứng LED
    update_led_display();

    // Bước 4: Cập nhật màn hình 7 đoạn
    update_7seg_display();
}

/* ============================================================================
 * PHÁT HIỆN CẠNH NÚT NHẤN
 * ============================================================================ */

/**
 * update_button_state() - Cập nhật trạng thái nút nhấn và phát hiện sự kiện
 *
 * PHÁT HIỆN CẠNH:
 * Phát hiện cạnh lên (nút vừa được nhấn):
 *   prevState == RELEASE && currState == PRESS → XỬ LÝ SỰ KIỆN
 *
 * NGĂN CHẶN:
 * - Kích hoạt nhiều lần khi giữ nút
 * - Mỗi lần nhấn = chính xác một sự kiện
 *
 * GỌI: Mỗi 10ms trong traffic_run()
 */
void update_button_state(void)
{
    for(int i = 0; i < 3; i++) {
        // Lưu trạng thái hiện tại làm trước đó
        prevState[i] = currState[i];

        // Đọc trạng thái mới từ phần cứng
        switch(i) {
            case 0:  // Nút MODE
                if(isButton1Pressed()) {
                    currState[i] = BTN_PRESS;
                } else {
                    currState[i] = BTN_RELEASE;
                }
                break;

            case 1:  // Nút MODIFY
                if(isButton2Pressed()) {
                    currState[i] = BTN_PRESS;
                } else {
                    currState[i] = BTN_RELEASE;
                }
                break;

            case 2:  // Nút SET
                if(isButton3Pressed()) {
                    currState[i] = BTN_PRESS;
                } else {
                    currState[i] = BTN_RELEASE;
                }
                break;
        }
    }
}



/* ============================================================================
 * CHẾ ĐỘ FSM 1 - HOẠT ĐỘNG TỰ ĐỘNG
 * ============================================================================ */

/**
 * fsm_normal_mode() - Máy trạng thái cho hoạt động tự động
 *
 * CHU KỲ GIAO THÔNG (4 trạng thái):
 *   INIT → RED_GREEN → RED_AMBER → GREEN_RED → AMBER_RED → (lặp lại)
 *
 * Cập nhật mỗi 1 giây (đếm ngược counter_road1/2)
 */
void fsm_normal_mode(void)
{
    static int timer_counter = 0;

    // Xử lý nút MODE - chuyển sang chế độ điều chỉnh
    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
        current_mode = MODE_2_RED_MODIFY;
        temp_duration = duration_RED;
        turn_off_all_leds();
        return;
    }

    // Đếm chu kỳ timer
    timer_counter++;
    if(timer_counter < TIMER_CYCLE) {
        return;  // Chưa đủ thời gian
    }
    timer_counter = 0;  // Reset cho chu kỳ tiếp theo

    // FSM giao thông - cập nhật mỗi giây
    switch(traffic_state) {
        case INIT:
            // Khởi tạo trạng thái đầu tiên
            traffic_state = RED_GREEN;
            counter_road1 = duration_RED;
            counter_road2 = duration_GREEN;
            break;

        case RED_GREEN:
            // Đường 1: ĐỎ, Đường 2: XANH
            counter_road1--;
            counter_road2--;

            if(counter_road2 <= 0) {
                traffic_state = RED_AMBER;
                counter_road1 = duration_AMBER;
                counter_road2 = duration_AMBER;
            }
            break;

        case RED_AMBER:
            // Đường 1: ĐỎ, Đường 2: VÀNG
            counter_road1--;
            counter_road2--;

            if(counter_road2 <= 0) {
                traffic_state = GREEN_RED;
                counter_road1 = duration_GREEN;
                counter_road2 = duration_RED;
            }
            break;

        case GREEN_RED:
            // Đường 1: XANH, Đường 2: ĐỎ
            counter_road1--;
            counter_road2--;

            if(counter_road1 <= 0) {
                traffic_state = AMBER_RED;
                counter_road1 = duration_AMBER;
                counter_road2 = duration_AMBER;
            }
            break;

        case AMBER_RED:
            // Đường 1: VÀNG, Đường 2: ĐỎ
            counter_road1--;
            counter_road2--;

            if(counter_road2 <= 0) {
                traffic_state = RED_GREEN;
                counter_road1 = duration_RED;
                counter_road2 = duration_GREEN;
            }
            break;
    }

    // Ngăn bộ đếm âm
    if(counter_road1 < 0) counter_road1 = 0;
    if(counter_road2 < 0) counter_road2 = 0;
}

/* ============================================================================
 * CHẾ ĐỘ FSM 2 - ĐIỀU CHỈNH THỜI GIAN ĐỎ
 * ============================================================================ */

/**
 * fsm_red_modify_mode() - Điều chỉnh thời gian đèn ĐỎ
 *
 * - Nhấp nháy LED ĐỎ
 * - Nút MODE: Chuyển sang CHẾ ĐỘ 3 (điều chỉnh VÀNG)
 * - Nút MODIFY: Tăng temp_duration (1→99→1)
 * - Nút SET: Lưu và tự động điều chỉnh thời gian khác
 */
void fsm_red_modify_mode(void)
{
    // Nút MODE - chuyển sang điều chỉnh VÀNG
    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
        current_mode = MODE_3_AMBER_MODIFY;
        temp_duration = duration_AMBER;
        return;
    }

    // Nút MODIFY - tăng giá trị
    if(currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE) {
        temp_duration++;
        if(temp_duration > 99) {
            temp_duration = 1;
        }
    }

    // Nút SET - lưu và tự động điều chỉnh
    if(currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE) {
        duration_RED = temp_duration;
        auto_adjust_duration(0);  // 0 = ĐỎ đã được sửa
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    // Nhấp nháy LED ĐỎ
    handle_led_blinking(0);  // 0 = ĐỎ
}

/* ============================================================================
 * CHẾ ĐỘ FSM 3 - ĐIỀU CHỈNH THỜI GIAN VÀNG
 * ============================================================================ */

/**
 * fsm_amber_modify_mode() - Điều chỉnh thời gian đèn VÀNG
 *
 * - Nhấp nháy LED VÀNG
 * - Nút MODE: Chuyển sang CHẾ ĐỘ 4 (điều chỉnh XANH)
 * - Nút MODIFY: Tăng temp_duration
 * - Nút SET: Lưu và tự động điều chỉnh
 */
void fsm_amber_modify_mode(void)
{
    // Nút MODE - chuyển sang điều chỉnh XANH
    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
        current_mode = MODE_4_GREEN_MODIFY;
        temp_duration = duration_GREEN;
        return;
    }

    // Nút MODIFY - tăng giá trị
    if(currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE) {
        temp_duration++;
        if(temp_duration > 99) temp_duration = 1;
    }

    // Nút SET - lưu và tự động điều chỉnh
    if(currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE) {
        duration_AMBER = temp_duration;
        auto_adjust_duration(1);  // 1 = VÀNG đã được sửa
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    // Nhấp nháy LED VÀNG
    handle_led_blinking(1);  // 1 = VÀNG
}

/* ============================================================================
 * CHẾ ĐỘ FSM 4 - ĐIỀU CHỈNH THỜI GIAN XANH
 * ============================================================================ */

/**
 * fsm_green_modify_mode() - Điều chỉnh thời gian đèn XANH
 *
 * - Nhấp nháy LED XANH
 * - Nút MODE: Quay về CHẾ ĐỘ 1 (không lưu)
 * - Nút MODIFY: Tăng temp_duration
 * - Nút SET: Lưu và tự động điều chỉnh
 */
void fsm_green_modify_mode(void)
{
    // Nút MODE - quay về chế độ tự động (không lưu)
    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    // Nút MODIFY - tăng giá trị
    if(currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE) {
        temp_duration++;
        if(temp_duration > 99) temp_duration = 1;
    }

    // Nút SET - lưu và tự động điều chỉnh
    if(currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE) {
        duration_GREEN = temp_duration;
        auto_adjust_duration(2);  // 2 = XANH đã được sửa
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    // Nhấp nháy LED XANH
    handle_led_blinking(2);  // 2 = XANH
}

/* ============================================================================
 * HÀM TỰ ĐỘNG ĐIỀU CHỈNH THỜI GIAN
 * ============================================================================ */

/**
 * auto_adjust_duration() - Tự động điều chỉnh các thời gian khác để duy trì ràng buộc
 *
 * RÀNG BUỘC: thời_gian_ĐỎ = thời_gian_XANH + thời_gian_VÀNG
 *
 * CHIẾN LƯỢC:
 * - Sửa ĐỎ (0): Giữ VÀNG, tính XANH = ĐỎ - VÀNG
 * - Sửa VÀNG (1): Cập nhật XANH = VÀNG + 4, tính ĐỎ = XANH + VÀNG
 * - Sửa XANH (2): Giữ VÀNG, tính ĐỎ = XANH + VÀNG
 *
 * THAM SỐ:
 *   modified_light: Đèn nào đã được sửa
 *                   0 = ĐỎ, 1 = VÀNG, 2 = XANH
 *
 * TRẢ VỀ:
 *   1: Đã điều chỉnh hoặc reset
 *   0: Không cần điều chỉnh (đã hợp lệ)
 *
 * MẶC ĐỊNH KHI RESET: ĐỎ=5, XANH=3, VÀNG=2
 */
int auto_adjust_duration(int modified_light)
{
    // Kiểm tra nếu ràng buộc đã được thỏa mãn
    if(duration_RED == (duration_GREEN + duration_AMBER)) {
        return 0;  // Không cần điều chỉnh
    }

    switch(modified_light) {
        case 0:  // ĐỎ đã được sửa
            // Chiến lược: Giữ VÀNG, tính XANH
            duration_GREEN = duration_RED - duration_AMBER;

            // Kiểm tra XANH hợp lệ
            if(duration_GREEN < 1 || duration_GREEN > 99) {
                duration_GREEN = duration_RED - duration_AMBER;
                duration_AMBER = duration_RED - duration_GREEN;

                // Kiểm tra VÀNG hợp lệ
                if(duration_AMBER < 1 || duration_AMBER > 99) {
                    // Reset về mặc định
                    duration_RED = 5;
                    duration_GREEN = 3;
                    duration_AMBER = 2;
                }
            }
            break;

        case 1:  // VÀNG đã được sửa
            // Chiến lược: XANH = VÀNG + 4, ĐỎ = XANH + VÀNG
            duration_GREEN = duration_AMBER + 4;
            duration_RED = duration_GREEN + duration_AMBER;

            // Kiểm tra nếu ĐỎ vượt giới hạn
            if(duration_RED > 99) {
                // Điều chỉnh để nằm trong giới hạn
                duration_AMBER = (99 - 3) / 2;  // = 48
                duration_GREEN = duration_AMBER + 3;  // = 51
                duration_RED = 99;

                if(duration_AMBER < 1) {
                    // Reset nếu không hợp lệ
                    duration_RED = 5;
                    duration_GREEN = 3;
                    duration_AMBER = 2;
                }
            }

            // Kiểm tra XANH hợp lệ
            if(duration_GREEN < 1 || duration_GREEN > 99) {
                duration_RED = 5;
                duration_GREEN = 3;
                duration_AMBER = 2;
            }
            break;

        case 2:  // XANH đã được sửa
            // Chiến lược: Giữ VÀNG, tính ĐỎ
            duration_RED = duration_GREEN + duration_AMBER;

            // Kiểm tra nếu ĐỎ vượt giới hạn
            if(duration_RED > 99) {
                // Giảm VÀNG để vừa
                duration_AMBER = 99 - duration_GREEN;
                duration_RED = 99;

                // Kiểm tra VÀNG hợp lệ
                if(duration_AMBER < 1) {
                    // Reset nếu không hợp lệ
                    duration_RED = 5;
                    duration_GREEN = 3;
                    duration_AMBER = 2;
                }
            }
            break;
    }

    return 1;  // Hoàn thành điều chỉnh
}


/* ============================================================================
 * SƠ ĐỒ HỆ THỐNG
 * ============================================================================ */

/*
 * CHUYỂN ĐỔI CHẾ ĐỘ:
 *
 *    CHẾ ĐỘ 1 (Tự động) ──[Nút MODE]──> CHẾ ĐỘ 2 (Điều chỉnh ĐỎ)
 *                                              │
 *                                        [Nút MODE]
 *                                              │
 *                                              ↓
 *                                        CHẾ ĐỘ 3 (Điều chỉnh VÀNG)
 *                                              │
 *                                        [Nút MODE]
 *                                              │
 *                                              ↓
 *                                        CHẾ ĐỘ 4 (Điều chỉnh XANH)
 *                                              │
 *                                   [Nút MODE hoặc SET]
 *                                              │
 *                                              ↓
 *    CHẾ ĐỘ 1 (Tự động) <───────────────────┘
 *
 *
 * CHU KỲ GIAO THÔNG (CHẾ ĐỘ 1):
 *
 *    INIT → RED_GREEN → RED_AMBER → GREEN_RED → AMBER_RED → (lặp lại)
 *           Đường1: ĐỎ   Đường1: ĐỎ   Đường1: XANH  Đường1: VÀNG
 *           Đường2: XANH Đường2: VÀNG Đường2: ĐỎ    Đường2: ĐỎ
 *
 *
 * DÒNG THỜI GIAN PHÁT HIỆN CẠNH:
 *
 *    Thời gian│ Vật lý  │ prevState │ currState │ Phát hiện│ Hành động
 *    ─────────┼─────────┼───────────┼───────────┼──────────┼───────────
 *    t=0ms    │  TẮT    │  RELEASE  │  RELEASE  │    -     │   -
 *    t=10ms   │  TẮT    │  RELEASE  │  RELEASE  │    -     │   -
 *    t=20ms   │  NHẤN   │  RELEASE  │  PRESS    │ CẠNH LÊN✓│ XỬ LÝ
 *    t=30ms   │  GIỮ    │  PRESS    │  PRESS    │    -     │ Bỏ qua
 *    t=40ms   │  GIỮ    │  PRESS    │  PRESS    │    -     │ Bỏ qua
 *    t=50ms   │  THẢ    │  PRESS    │  RELEASE  │ CẠNH XUỐNG│Bỏ qua
 */
