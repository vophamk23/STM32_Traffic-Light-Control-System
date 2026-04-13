/*
 * fsm_traffic.c - Điều khiển đèn giao thông 2 chiều
 * Ràng buộc: RED = GREEN + AMBER
 * Nút: MODE (chuyển chế độ), MODIFY (tăng giá trị), SET (lưu)
 * Tốc độ cập nhật: 10ms (100Hz)
 */

#include "fsm_traffic.h"

/* ============================================================================
 * KHỞI TẠO
 * ============================================================================ */
void traffic_init(void)
{
    duration_RED = 5;
    duration_AMBER = 2;
    duration_GREEN = 3;

    current_mode = MODE_1_NORMAL;
    traffic_state = INIT;

    counter_road1 = 0;
    counter_road2 = 0;

    turn_off_all_leds();

    for (int i = 0; i < 3; i++)
    {
        prevState[i] = BTN_RELEASE;
        currState[i] = BTN_RELEASE;
    }

    blink_counter = 0;
    flag_blink = 0;

    flagRed[0] = flagRed[1] = 0;
    flagGreen[0] = flagGreen[1] = 0;
    flagYellow[0] = flagYellow[1] = 0;

    temp_duration = 0;
}

/* ============================================================================
 * HÀM CHÍNH - gọi mỗi 10ms từ Task_Traffic_FSM
 * ============================================================================ */
void traffic_run(void)
{
    update_button_state();

    switch (current_mode)
    {
    case MODE_1_NORMAL:
        fsm_normal_mode();
        break;
    case MODE_2_RED_MODIFY:
        fsm_red_modify_mode();
        break;
    case MODE_3_AMBER_MODIFY:
        fsm_amber_modify_mode();
        break;
    case MODE_4_GREEN_MODIFY:
        fsm_green_modify_mode();
        break;
    }
    // Không gọi update_led/7seg ở đây - Task_Update_Display đã lo
}

/* ============================================================================
 * PHÁT HIỆN CẠNH NÚT NHẤN
 * ============================================================================ */
void update_button_state(void)
{
    for (int i = 0; i < 3; i++)
    {
        prevState[i] = currState[i];

        int pressed = 0;
        switch (i)
        {
        case 0:
            pressed = isButton1Pressed();
            break;
        case 1:
            pressed = isButton2Pressed();
            break;
        case 2:
            pressed = isButton3Pressed();
            break;
        }
        currState[i] = pressed ? BTN_PRESS : BTN_RELEASE;
    }
}

/* ============================================================================
 * MODE 1 - TỰ ĐỘNG
 * Chu kỳ: INIT → RED_GREEN → RED_AMBER → GREEN_RED → AMBER_RED → lặp
 * ============================================================================ */
void fsm_normal_mode(void)
{
    static int timer_counter = 0;

    // Nút MODE → sang Mode 2
    if (currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE)
    {
        current_mode = MODE_2_RED_MODIFY;
        temp_duration = duration_RED;
        turn_off_all_leds();
        return;
    }

    if (++timer_counter < TIMER_CYCLE)
        return;
    timer_counter = 0;

    switch (traffic_state)
    {
    case INIT:
        traffic_state = RED_GREEN;
        counter_road1 = duration_RED;
        counter_road2 = duration_GREEN;
        break;

    case RED_GREEN:
        counter_road1--;
        counter_road2--;
        if (counter_road2 <= 0)
        {
            traffic_state = RED_AMBER;
            counter_road1 = duration_AMBER;
            counter_road2 = duration_AMBER;
        }
        break;

    case RED_AMBER:
        counter_road1--;
        counter_road2--;
        if (counter_road2 <= 0)
        {
            traffic_state = GREEN_RED;
            counter_road1 = duration_GREEN;
            counter_road2 = duration_RED;
        }
        break;

    case GREEN_RED:
        counter_road1--;
        counter_road2--;
        if (counter_road1 <= 0)
        {
            traffic_state = AMBER_RED;
            counter_road1 = duration_AMBER;
            counter_road2 = duration_AMBER;
        }
        break;

    case AMBER_RED:
        counter_road1--;
        counter_road2--;
        if (counter_road2 <= 0)
        {
            traffic_state = RED_GREEN;
            counter_road1 = duration_RED;
            counter_road2 = duration_GREEN;
        }
        break;
    }

    if (counter_road1 < 0)
        counter_road1 = 0;
    if (counter_road2 < 0)
        counter_road2 = 0;
}

/* ============================================================================
 * MODE 2 - ĐIỀU CHỈNH ĐỎ (đèn đỏ nhấp nháy)
 * ============================================================================ */
void fsm_red_modify_mode(void)
{
    if (currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE)
    {
        current_mode = MODE_3_AMBER_MODIFY;
        temp_duration = duration_AMBER;
        return;
    }

    if (currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE)
    {
        if (++temp_duration > 99)
            temp_duration = 1;
    }

    if (currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE)
    {
        duration_RED = temp_duration;
        auto_adjust_duration(0);
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    handle_led_blinking(0); // 0 = ĐỎ
}

/* ============================================================================
 * MODE 3 - ĐIỀU CHỈNH VÀNG (đèn vàng nhấp nháy)
 * ============================================================================ */
void fsm_amber_modify_mode(void)
{
    if (currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE)
    {
        current_mode = MODE_4_GREEN_MODIFY;
        temp_duration = duration_GREEN;
        return;
    }

    if (currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE)
    {
        if (++temp_duration > 99)
            temp_duration = 1;
    }

    if (currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE)
    {
        duration_AMBER = temp_duration;
        auto_adjust_duration(1);
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    handle_led_blinking(1); // 1 = VÀNG
}

/* ============================================================================
 * MODE 4 - ĐIỀU CHỈNH XANH (đèn xanh nhấp nháy)
 * MODE nhấn → về Mode 1 KHÔNG lưu
 * ============================================================================ */
void fsm_green_modify_mode(void)
{
    if (currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE)
    {
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    if (currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE)
    {
        if (++temp_duration > 99)
            temp_duration = 1;
    }

    if (currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE)
    {
        duration_GREEN = temp_duration;
        auto_adjust_duration(2);
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    handle_led_blinking(2); // 2 = XANH
}

/* ============================================================================
 * TỰ ĐỘNG ĐIỀU CHỈNH THỜI GIAN - duy trì RED = GREEN + AMBER
 * modified_light: 0=ĐỎ, 1=VÀNG, 2=XANH
 * ============================================================================ */
int auto_adjust_duration(int modified_light)
{
    if (duration_RED == (duration_GREEN + duration_AMBER))
        return 0;

    switch (modified_light)
    {
    case 0: // Sửa ĐỎ → tính lại XANH, giữ VÀNG
        duration_GREEN = duration_RED - duration_AMBER;
        if (duration_GREEN < 1 || duration_GREEN > 99)
        {
            duration_RED = 5;
            duration_GREEN = 3;
            duration_AMBER = 2;
        }
        break;

    case 1: // Sửa VÀNG → XANH = VÀNG + 4, ĐỎ = XANH + VÀNG
        duration_GREEN = duration_AMBER + 4;
        duration_RED = duration_GREEN + duration_AMBER;
        if (duration_RED > 99)
        {
            duration_AMBER = (99 - 3) / 2;
            duration_GREEN = duration_AMBER + 3;
            duration_RED = 99;
        }
        if (duration_GREEN < 1 || duration_GREEN > 99)
        {
            duration_RED = 5;
            duration_GREEN = 3;
            duration_AMBER = 2;
        }
        break;

    case 2: // Sửa XANH → tính lại ĐỎ, giữ VÀNG
        duration_RED = duration_GREEN + duration_AMBER;
        if (duration_RED > 99)
        {
            duration_AMBER = 99 - duration_GREEN;
            duration_RED = 99;
            if (duration_AMBER < 1)
            {
                duration_RED = 5;
                duration_GREEN = 3;
                duration_AMBER = 2;
            }
        }
        break;
    }

    return 1;
}