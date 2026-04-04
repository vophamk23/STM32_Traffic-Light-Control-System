/*
 * led_display.c
 * Module triển khai xử lý các LED đơn cho hệ thống đèn giao thông
 * Điều khiển 6 LED: RED1, YELLOW1, GREEN1, RED2, YELLOW2, GREEN2
 */

#include "led_display.h"

/* ==================================================================
 * CẬP NHẬT LED DISPLAY
 * ================================================================== */

/**
 * Hàm cập nhật hiển thị LED dựa trên chế độ hiện tại
 *
 * Logic:
 * - MODE_1_NORMAL: Hiển thị đèn giao thông theo traffic_state
 * - MODE 2/3/4: Hiển thị LED nhấp nháy dựa trên các flag
 *
 * Được gọi: Liên tục trong vòng lặp chính hoặc từ timer
 */
void update_led_display(void)
{
    // ============ CHẾ ĐỘ HOẠT ĐỘNG BÌNH THƯỜNG ============
    if (current_mode == MODE_1_NORMAL)
    {
        // Hiển thị đèn giao thông theo trạng thái finite state machine

        switch (traffic_state)
        {
        case INIT:               // Trạng thái khởi tạo
            turn_off_all_leds(); // Tắt hết tất cả LED
            break;

        case RED_GREEN:                  // Đường 1: ĐỎ, Đường 2: XANH
            set_traffic_led(0, 1, 0, 0); // Đường 1: chỉ đỏ sáng
            set_traffic_led(1, 0, 0, 1); // Đường 2: chỉ xanh sáng
            break;

        case RED_AMBER:                  // Đường 1: ĐỎ, Đường 2: VÀNG
            set_traffic_led(0, 1, 0, 0); // Đường 1: chỉ đỏ sáng
            set_traffic_led(1, 0, 1, 0); // Đường 2: chỉ vàng sáng
            break;

        case GREEN_RED:                  // Đường 1: XANH, Đường 2: ĐỎ
            set_traffic_led(0, 0, 0, 1); // Đường 1: chỉ xanh sáng
            set_traffic_led(1, 1, 0, 0); // Đường 2: chỉ đỏ sáng
            break;

        case AMBER_RED:                  // Đường 1: VÀNG, Đường 2: ĐỎ
            set_traffic_led(0, 0, 1, 0); // Đường 1: chỉ vàng sáng
            set_traffic_led(1, 1, 0, 0); // Đường 2: chỉ đỏ sáng
            break;
        }
    }
    // ============ CHẾ ĐỘ ĐIỀU CHỈNH (MODE 2/3/4) ============
    else
    {
        // Hiển thị LED dựa trên các flag được cập nhật bởi handle_led_blinking()
        // Các flag này được thay đổi mỗi 500ms để tạo hiệu ứng nhấp nháy

        // Cập nhật đèn đỏ cả 2 đường
        displayLED_RED(flagRed[0], 0); // Đèn đỏ đường 1
        displayLED_RED(flagRed[1], 1); // Đèn đỏ đường 2

        // Cập nhật đèn vàng cả 2 đường
        displayLED_YELLOW(flagYellow[0], 0); // Đèn vàng đường 1
        displayLED_YELLOW(flagYellow[1], 1); // Đèn vàng đường 2

        // Cập nhật đèn xanh cả 2 đường
        displayLED_GREEN(flagGreen[0], 0); // Đèn xanh đường 1
        displayLED_GREEN(flagGreen[1], 1); // Đèn xanh đường 2
    }
}


/* ==================================================================
 * ĐIỀU KHIỂN LED CƠ BẢN
 * ================================================================== */

/**
 * Hàm tắt TẤT CẢ các LED giao thông
 *
 * Chức năng:
 * - Tắt 6 LED của 2 đường (3 LED mỗi đường)
 * - Sử dụng khi khởi tạo hoặc chuyển trạng thái
 */
void turn_off_all_leds(void)
{
    // Tắt LED đường 1
    HAL_GPIO_WritePin(GPIOA, RED1_Pin, GPIO_PIN_RESET);    // Tắt đèn đỏ 1
    HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, GPIO_PIN_RESET); // Tắt đèn vàng 1
    HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, GPIO_PIN_RESET);  // Tắt đèn xanh 1

    // Tắt LED đường 2
    HAL_GPIO_WritePin(GPIOA, RED2_Pin, GPIO_PIN_RESET);    // Tắt đèn đỏ 2
    HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, GPIO_PIN_RESET); // Tắt đèn vàng 2
    HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, GPIO_PIN_RESET);  // Tắt đèn xanh 2
}

/**
 * Hàm thiết lập trạng thái đèn giao thông cho một đường
 *
 * @param road: Chỉ số đường (0 = Đường 1, 1 = Đường 2)
 * @param red: Trạng thái đèn đỏ (1 = sáng, 0 = tắt)
 * @param amber: Trạng thái đèn vàng (1 = sáng, 0 = tắt)
 * @param green: Trạng thái đèn xanh (1 = sáng, 0 = tắt)
 *
 * LƯU Ý: Logic đảo ngược!
 * - Tham số = 1 (sáng) → GPIO_PIN_RESET (vì LED active LOW)
 * - Tham số = 0 (tắt) → GPIO_PIN_SET
 */
void set_traffic_led(int road, int red, int amber, int green)
{
    if (road == 0)
    { // ĐƯỜNG 1
        // Đèn đỏ: nếu red=1 thì RESET (sáng), nếu red=0 thì SET (tắt)
        HAL_GPIO_WritePin(GPIOA, RED1_Pin, red ? GPIO_PIN_RESET : GPIO_PIN_SET);

        // Đèn vàng: nếu amber=1 thì RESET (sáng), nếu amber=0 thì SET (tắt)
        HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, amber ? GPIO_PIN_RESET : GPIO_PIN_SET);

        // Đèn xanh: nếu green=1 thì RESET (sáng), nếu green=0 thì SET (tắt)
        HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, green ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }
    else
    { // ĐƯỜNG 2
        // Tương tự như đường 1 nhưng dùng các chân LED của đường 2
        HAL_GPIO_WritePin(GPIOA, RED2_Pin, red ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, amber ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, green ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }
}

/**
 * Hàm điều khiển ĐÈN ĐỎ của một đường cụ thể
 *
 * @param IS_ON: Trạng thái mong muốn (1 = sáng, 0 = tắt)
 * @param index: Chỉ số đường (0 = Đường 1, 1 = Đường 2)
 *
 * Sử dụng: Chủ yếu trong chế độ blinking (nhấp nháy)
 */
void displayLED_RED(int IS_ON, int index)
{
    switch (index)
    {
    case 0: // Đường 1
        // IS_ON=1 → SET (sáng), IS_ON=0 → RESET (tắt)
        HAL_GPIO_WritePin(GPIOA, RED1_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;

    case 1: // Đường 2
        HAL_GPIO_WritePin(GPIOA, RED2_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    }
}

/**
 * Hàm điều khiển ĐÈN VÀNG của một đường cụ thể
 *
 * @param IS_ON: Trạng thái mong muốn (1 = sáng, 0 = tắt)
 * @param index: Chỉ số đường (0 = Đường 1, 1 = Đường 2)
 */
void displayLED_YELLOW(int IS_ON, int index)
{
    switch (index)
    {
    case 0: // Đường 1
        HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;

    case 1: // Đường 2
        HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    }
}

/**
 * Hàm điều khiển ĐÈN XANH của một đường cụ thể
 *
 * @param IS_ON: Trạng thái mong muốn (1 = sáng, 0 = tắt)
 * @param index: Chỉ số đường (0 = Đường 1, 1 = Đường 2)
 */
void displayLED_GREEN(int IS_ON, int index)
{
    switch (index)
    {
    case 0: // Đường 1
        HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;

    case 1: // Đường 2
        HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    }
}

/* ==================================================================
 * XỬ LÝ LED BLINKING (NHẤP NHÁY) - 2Hz = 500ms BẬT/TẮT
 * ================================================================== */
/**
 * Hàm xử lý hiệu ứng nhấp nháy LED trong chế độ điều chỉnh
 *
 * @param led_type: Loại LED cần nhấp nháy
 *                  0 = GREEN (Mode 4)
 *                  1 = YELLOW (Mode 3)
 *                  2 = RED (Mode 2)
 *
 * Cơ chế hoạt động:
 * - Được gọi mỗi 10ms từ timer interrupt
 * - Sau 50 lần gọi (500ms) → đổi trạng thái LED
 * - Chu kỳ: 500ms sáng + 500ms tắt = 1 giây (tần số 1Hz)
 * - Chỉ LED đang được điều chỉnh mới nhấp nháy, các LED khác TẮT
 */
void handle_led_blinking(int led_type)
{
    // Tăng bộ đếm nhấp nháy
    blink_counter++;
    // Kiểm tra đã đủ thời gian chưa (50 x 10ms = 500ms)
    if (blink_counter >= MAX_BLINK_COUNTER)
    {
        // Reset bộ đếm về 0 để bắt đầu chu kỳ mới
        blink_counter = 0;
        // Đảo trạng thái cờ nhấp nháy: 0→1 hoặc 1→0
        // flag_blink = 0: LED tắt trong chu kỳ này
        // flag_blink = 1: LED sáng trong chu kỳ này
        flag_blink = !flag_blink;
        // ============ BƯỚC 1: TẮT TẤT CẢ CÁC LED ============
        // Set tất cả flag = 1 (tắt) vì active LOW
        // Điều này đảm bảo chỉ có LED đang điều chỉnh mới có thể sáng
        flagRed[0] = 1;    // Tắt đèn đỏ đường 1
        flagRed[1] = 1;    // Tắt đèn đỏ đường 2
        flagGreen[0] = 1;  // Tắt đèn xanh đường 1
        flagGreen[1] = 1;  // Tắt đèn xanh đường 2
        flagYellow[0] = 1; // Tắt đèn vàng đường 1
        flagYellow[1] = 1; // Tắt đèn vàng đường 2
        // ============ BƯỚC 2: CHỈ BẬT LED ĐANG ĐIỀU CHỈNH ============
        // Dựa vào led_type để quyết định LED nào được nhấp nháy
        switch (led_type)
        {
        case 0: // MODE 2: Điều chỉnh thời gian ĐÈN ĐỎ
            // CHỈ đèn đỏ CẢ 2 ĐƯỜNG nhấp nháy đồng thời
            flagRed[0] = flag_blink;
            flagRed[1] = flag_blink;
            break;
        case 1: // MODE 3: Điều chỉnh thời gian ĐÈN VÀNG
                    // CHỈ đèn vàng CẢ 2 ĐƯỜNG nhấp nháy đồng thời
                    flagYellow[0] = flag_blink;
                    flagYellow[1] = flag_blink;
                    break;
        case 2: // MODE 4: Điều chỉnh thời gian ĐÈN XANH
            // CHỈ đèn xanh CẢ 2 ĐƯỜNG nhấp nháy đồng thời
            flagGreen[0] = flag_blink; // 0=tắt, 1=sáng theo chu kỳ
            flagGreen[1] = flag_blink;

            break;
        }
    }
    // Nếu chưa đủ 500ms thì không làm gì, chỉ tăng counter
}


/* ==================================================================
 * KẾT THÚC FILE
 * ================================================================== */
