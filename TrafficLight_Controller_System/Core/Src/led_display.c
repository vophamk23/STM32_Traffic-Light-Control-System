/*
 * led_display.c - Điều khiển 6 LED đèn giao thông (3 màu x 2 đường)
 * LƯU Ý: LED active LOW → muốn sáng dùng GPIO_PIN_RESET
 */

#include "led_display.h"

/* ============================================================
 * update_led_display - Cập nhật LED theo mode hiện tại
 * Mode 1: Hiển thị theo traffic_state
 * Mode 2/3/4: Hiển thị LED nhấp nháy (flagRed/Yellow/Green)
 * ============================================================ */
void update_led_display(void)
{
    if (current_mode == MODE_1_NORMAL)
    {
        switch (traffic_state)
        {
        case INIT:
            turn_off_all_leds();
            break;
        case RED_GREEN:
            set_traffic_led(0, 1, 0, 0); // Đường 1: Đỏ
            set_traffic_led(1, 0, 0, 1); // Đường 2: Xanh
            break;
        case RED_AMBER:
            set_traffic_led(0, 1, 0, 0); // Đường 1: Đỏ
            set_traffic_led(1, 0, 1, 0); // Đường 2: Vàng
            break;
        case GREEN_RED:
            set_traffic_led(0, 0, 0, 1); // Đường 1: Xanh
            set_traffic_led(1, 1, 0, 0); // Đường 2: Đỏ
            break;
        case AMBER_RED:
            set_traffic_led(0, 0, 1, 0); // Đường 1: Vàng
            set_traffic_led(1, 1, 0, 0); // Đường 2: Đỏ
            break;
        }
    }
    else
    {
        // Chế độ điều chỉnh: dùng flag được cập nhật bởi handle_led_blinking()
        displayLED_RED(flagRed[0], 0);
        displayLED_RED(flagRed[1], 1);
        displayLED_YELLOW(flagYellow[0], 0);
        displayLED_YELLOW(flagYellow[1], 1);
        displayLED_GREEN(flagGreen[0], 0);
        displayLED_GREEN(flagGreen[1], 1);
    }
}

/* ============================================================
 * turn_off_all_leds - Tắt toàn bộ 6 LED
 * ============================================================ */
void turn_off_all_leds(void)
{
    HAL_GPIO_WritePin(GPIOA, RED1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, RED2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, GPIO_PIN_RESET);
}

/* ============================================================
 * set_traffic_led - Đặt trạng thái 3 đèn của 1 đường
 *   road : 0 = Đường 1, 1 = Đường 2
 *   red/amber/green : 1 = sáng, 0 = tắt
 * LƯU Ý: LED active LOW → 1 (sáng) → GPIO_RESET
 * ============================================================ */
void set_traffic_led(int road, int red, int amber, int green)
{
    if (road == 0)
    {
        HAL_GPIO_WritePin(GPIOA, RED1_Pin, red ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, amber ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, green ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOA, RED2_Pin, red ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, amber ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, green ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }
}

/* Điều khiển từng màu cho 1 đường (dùng trong chế độ blinking) */
void displayLED_RED(int IS_ON, int index)
{
    GPIO_PinState state = IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(GPIOA, (index == 0) ? RED1_Pin : RED2_Pin, state);
}

void displayLED_YELLOW(int IS_ON, int index)
{
    GPIO_PinState state = IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(GPIOA, (index == 0) ? YELLOW1_Pin : YELLOW2_Pin, state);
}

void displayLED_GREEN(int IS_ON, int index)
{
    GPIO_PinState state = IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(GPIOA, (index == 0) ? GREEN1_Pin : GREEN2_Pin, state);
}

/* ============================================================
 * handle_led_blinking - Tạo hiệu ứng nhấp nháy 1Hz (500ms bật/tắt)
 *   led_type: 0=ĐỎ (Mode 2), 1=VÀNG (Mode 3), 2=XANH (Mode 4)
 * Gọi mỗi 10ms. Sau MAX_BLINK_COUNTER (50) lần → đảo flag_blink
 * ============================================================ */
void handle_led_blinking(int led_type)
{
    if (++blink_counter < MAX_BLINK_COUNTER)
        return;

    blink_counter = 0;
    flag_blink = !flag_blink;

    // Tắt hết trước
    flagRed[0] = flagRed[1] = 1;
    flagGreen[0] = flagGreen[1] = 1;
    flagYellow[0] = flagYellow[1] = 1;

    // Chỉ bật LED đang điều chỉnh
    switch (led_type)
    {
    case 0: // Mode 2: nhấp nháy đèn ĐỎ
        flagRed[0] = flagRed[1] = flag_blink;
        break;
    case 1: // Mode 3: nhấp nháy đèn VÀNG
        flagYellow[0] = flagYellow[1] = flag_blink;
        break;
    case 2: // Mode 4: nhấp nháy đèn XANH
        flagGreen[0] = flagGreen[1] = flag_blink;
        break;
    }
}