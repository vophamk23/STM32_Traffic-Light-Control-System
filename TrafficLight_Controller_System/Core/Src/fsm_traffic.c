/*
 * fsm_traffic.c - Điều khiển đèn giao thông 2 đường, 4 chế độ
 *
 * Mode 1 (Normal): Tự động chuyển đèn theo chu kỳ
 * Mode 2/3/4: Điều chỉnh thời gian ĐỎ / VÀNG / XANH
 *
 * Nút: MODE = chuyển chế độ, MODIFY = tăng giá trị, SET = lưu & về Mode 1
 * Ràng buộc: duration_RED = duration_GREEN + duration_AMBER
 */

#include "fsm_traffic.h"

/* ============================================================
 * KHỞI TẠO
 * ============================================================ */
void traffic_init(void)
{
    duration_RED   = 5;
    duration_AMBER = 2;
    duration_GREEN = 3;

    current_mode  = MODE_1_NORMAL;
    traffic_state = INIT;

    counter_road1 = 0;
    counter_road2 = 0;

    turn_off_all_leds();

    for (int i = 0; i < 3; i++) {
        prevState[i] = BTN_RELEASE;
        currState[i] = BTN_RELEASE;
    }

    blink_counter = 0;
    flag_blink    = 0;

    flagRed[0]    = flagRed[1]    = 0;
    flagGreen[0]  = flagGreen[1]  = 0;
    flagYellow[0] = flagYellow[1] = 0;

    temp_duration = 0;
}

/* ============================================================
 * HÀM CHÍNH - gọi mỗi 10ms bởi Task_Traffic_FSM
 * ============================================================ */
void traffic_run(void)
{
    // Bước 1: Đọc trạng thái nút nhấn
    update_button_state();

    // Bước 2: Xử lý logic theo chế độ
    // (Display được cập nhật riêng bởi Task_Update_Display)
    switch(current_mode) {
        case MODE_1_NORMAL:        fsm_normal_mode();       break;
        case MODE_2_RED_MODIFY:    fsm_red_modify_mode();   break;
        case MODE_3_AMBER_MODIFY:  fsm_amber_modify_mode(); break;
        case MODE_4_GREEN_MODIFY:  fsm_green_modify_mode(); break;
    }
}

/* ============================================================
 * PHÁT HIỆN CẠNH NÚT NHẤN
 * Đọc cờ từ button.c → cập nhật prevState/currState
 * prevState=RELEASE + currState=PRESS → sự kiện nhấn 1 lần
 * ============================================================ */
void update_button_state(void)
{
    for (int i = 0; i < 3; i++) {
        prevState[i] = currState[i];
        switch (i) {
            case 0: currState[i] = isButton1Pressed() ? BTN_PRESS : BTN_RELEASE; break;
            case 1: currState[i] = isButton2Pressed() ? BTN_PRESS : BTN_RELEASE; break;
            case 2: currState[i] = isButton3Pressed() ? BTN_PRESS : BTN_RELEASE; break;
        }
    }
}

/* Macro phát hiện cạnh lên (nút vừa nhấn) */
#define JUST_PRESSED(i) (currState[i] == BTN_PRESS && prevState[i] == BTN_RELEASE)

/* ============================================================
 * MODE 1 - TỰ ĐỘNG
 * Chu kỳ: INIT → RED_GREEN → RED_AMBER → GREEN_RED → AMBER_RED → (lặp)
 * Cập nhật mỗi giây (đếm TIMER_CYCLE tick = 100 x 10ms)
 * ============================================================ */
void fsm_normal_mode(void)
{
    static int timer_counter = 0;

    // Nhấn MODE → vào chế độ chỉnh thời gian ĐỎ
    if (JUST_PRESSED(0)) {
        current_mode  = MODE_2_RED_MODIFY;
        temp_duration = duration_RED;
        turn_off_all_leds();
        return;
    }

    // Chỉ cập nhật mỗi 1 giây
    if (++timer_counter < TIMER_CYCLE) return;
    timer_counter = 0;

    switch (traffic_state) {
        case INIT:
            traffic_state = RED_GREEN;
            counter_road1 = duration_RED;
            counter_road2 = duration_GREEN;
            break;

        case RED_GREEN:
            counter_road1--;
            counter_road2--;
            if (counter_road2 <= 0) {
                traffic_state = RED_AMBER;
                counter_road1 = duration_AMBER;
                counter_road2 = duration_AMBER;
            }
            break;

        case RED_AMBER:
            counter_road1--;
            counter_road2--;
            if (counter_road2 <= 0) {
                traffic_state = GREEN_RED;
                counter_road1 = duration_GREEN;
                counter_road2 = duration_RED;
            }
            break;

        case GREEN_RED:
            counter_road1--;
            counter_road2--;
            if (counter_road1 <= 0) {
                traffic_state = AMBER_RED;
                counter_road1 = duration_AMBER;
                counter_road2 = duration_AMBER;
            }
            break;

        case AMBER_RED:
            counter_road1--;
            counter_road2--;
            if (counter_road2 <= 0) {
                traffic_state = RED_GREEN;
                counter_road1 = duration_RED;
                counter_road2 = duration_GREEN;
            }
            break;
    }

    if (counter_road1 < 0) counter_road1 = 0;
    if (counter_road2 < 0) counter_road2 = 0;
}

/* ============================================================
 * MODE 2 - CHỈNH THỜI GIAN ĐỎ
 * ============================================================ */
void fsm_red_modify_mode(void)
{
    if (JUST_PRESSED(0)) {  // MODE → sang chỉnh VÀNG
        current_mode  = MODE_3_AMBER_MODIFY;
        temp_duration = duration_AMBER;
        return;
    }
    if (JUST_PRESSED(1)) {  // MODIFY → tăng giá trị
        if (++temp_duration > 99) temp_duration = 1;
    }
    if (JUST_PRESSED(2)) {  // SET → lưu & về Mode 1
        duration_RED = temp_duration;
        auto_adjust_duration(0);
        current_mode  = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }
    handle_led_blinking(0);  // Nhấp nháy đèn ĐỎ
}

/* ============================================================
 * MODE 3 - CHỈNH THỜI GIAN VÀNG
 * ============================================================ */
void fsm_amber_modify_mode(void)
{
    if (JUST_PRESSED(0)) {  // MODE → sang chỉnh XANH
        current_mode  = MODE_4_GREEN_MODIFY;
        temp_duration = duration_GREEN;
        return;
    }
    if (JUST_PRESSED(1)) {
        if (++temp_duration > 99) temp_duration = 1;
    }
    if (JUST_PRESSED(2)) {
        duration_AMBER = temp_duration;
        auto_adjust_duration(1);
        current_mode  = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }
    handle_led_blinking(1);  // Nhấp nháy đèn VÀNG
}

/* ============================================================
 * MODE 4 - CHỈNH THỜI GIAN XANH
 * ============================================================ */
void fsm_green_modify_mode(void)
{
    if (JUST_PRESSED(0)) {  // MODE → về Mode 1 (không lưu)
        current_mode  = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }
    if (JUST_PRESSED(1)) {
        if (++temp_duration > 99) temp_duration = 1;
    }
    if (JUST_PRESSED(2)) {
        duration_GREEN = temp_duration;
        auto_adjust_duration(2);
        current_mode  = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }
    handle_led_blinking(2);  // Nhấp nháy đèn XANH
}

/* ============================================================
 * TỰ ĐỘNG ĐIỀU CHỈNH để duy trì: RED = GREEN + AMBER
 *   modified_light: 0=ĐỎ vừa sửa, 1=VÀNG vừa sửa, 2=XANH vừa sửa
 * Nếu giá trị tính được nằm ngoài [1,99] → reset mặc định (5/2/3)
 * ============================================================ */
int auto_adjust_duration(int modified_light)
{
    // Không cần điều chỉnh nếu ràng buộc đã thỏa
    if (duration_RED == duration_GREEN + duration_AMBER) return 0;

    switch (modified_light) {
        case 0:  // Sửa ĐỎ → giữ VÀNG, tính XANH = ĐỎ - VÀNG
            duration_GREEN = duration_RED - duration_AMBER;
            if (duration_GREEN < 1 || duration_GREEN > 99) {
                duration_RED = 5; duration_AMBER = 2; duration_GREEN = 3;
            }
            break;

        case 1:  // Sửa VÀNG → XANH = VÀNG + 4, ĐỎ = XANH + VÀNG
            duration_GREEN = duration_AMBER + 4;
            duration_RED   = duration_GREEN + duration_AMBER;
            if (duration_RED > 99 || duration_GREEN < 1 || duration_GREEN > 99) {
                duration_RED = 5; duration_AMBER = 2; duration_GREEN = 3;
            }
            break;

        case 2:  // Sửa XANH → giữ VÀNG, tính ĐỎ = XANH + VÀNG
            duration_RED = duration_GREEN + duration_AMBER;
            if (duration_RED > 99) {
                duration_AMBER = 99 - duration_GREEN;
                duration_RED   = 99;
                if (duration_AMBER < 1) {
                    duration_RED = 5; duration_AMBER = 2; duration_GREEN = 3;
                }
            }
            break;
    }

    return 1;
}