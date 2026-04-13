/*
 * led_display.c - Điều khiển 6 LED đèn giao thông (active LOW)
 * RED1, YELLOW1, GREEN1, RED2, YELLOW2, GREEN2
 * IS_ON=1 → GPIO_RESET (sáng), IS_ON=0 → GPIO_SET (tắt)
 */

#include "led_display.h"

/* ============================================================================
 * CẬP NHẬT LED DISPLAY
 * ============================================================================ */
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
            set_traffic_led(0, 1, 0, 0);
            set_traffic_led(1, 0, 0, 1);
            break;
        case RED_AMBER:
            set_traffic_led(0, 1, 0, 0);
            set_traffic_led(1, 0, 1, 0);
            break;
        case GREEN_RED:
            set_traffic_led(0, 0, 0, 1);
            set_traffic_led(1, 1, 0, 0);
            break;
        case AMBER_RED:
            set_traffic_led(0, 0, 1, 0);
            set_traffic_led(1, 1, 0, 0);
            break;
        }
    }
    else
    {
        // Mode 2/3/4: hiển thị theo flag từ handle_led_blinking()
        displayLED_RED(flagRed[0], 0);
        displayLED_RED(flagRed[1], 1);
        displayLED_YELLOW(flagYellow[0], 0);
        displayLED_YELLOW(flagYellow[1], 1);
        displayLED_GREEN(flagGreen[0], 0);
        displayLED_GREEN(flagGreen[1], 1);
    }
}

/* ============================================================================
 * ĐIỀU KHIỂN LED CƠ BẢN
 * ============================================================================ */
void turn_off_all_leds(void)
{
    HAL_GPIO_WritePin(GPIOA, RED1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, RED2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, GPIO_PIN_SET);
}

// road: 0=Đường1, 1=Đường2 | red/amber/green: 1=sáng, 0=tắt
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

// IS_ON: 1=sáng, 0=tắt | index: 0=Đường1, 1=Đường2
void displayLED_RED(int IS_ON, int index)
{
    uint16_t pin = (index == 0) ? RED1_Pin : RED2_Pin;
    HAL_GPIO_WritePin(GPIOA, pin, IS_ON ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void displayLED_YELLOW(int IS_ON, int index)
{
    uint16_t pin = (index == 0) ? YELLOW1_Pin : YELLOW2_Pin;
    HAL_GPIO_WritePin(GPIOA, pin, IS_ON ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void displayLED_GREEN(int IS_ON, int index)
{
    uint16_t pin = (index == 0) ? GREEN1_Pin : GREEN2_Pin;
    HAL_GPIO_WritePin(GPIOA, pin, IS_ON ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/* ============================================================================
 * LED BLINKING - 1Hz (500ms bật / 500ms tắt)
 * led_type: 0=ĐỎ, 1=VÀNG, 2=XANH
 * Gọi mỗi 10ms → đổi trạng thái sau 50 lần (500ms)
 * ============================================================================ */
void handle_led_blinking(int led_type)
{
    if (++blink_counter < MAX_BLINK_COUNTER)
        return;

    blink_counter = 0;
    flag_blink = !flag_blink;

    // Tắt tất cả (IS_ON=0)
    flagRed[0] = flagRed[1] = 0;
    flagGreen[0] = flagGreen[1] = 0;
    flagYellow[0] = flagYellow[1] = 0;

    // Chỉ bật LED đang điều chỉnh
    switch (led_type)
    {
    case 0:
        flagRed[0] = flagRed[1] = flag_blink;
        break;
    case 1:
        flagYellow[0] = flagYellow[1] = flag_blink;
        break;
    case 2:
        flagGreen[0] = flagGreen[1] = flag_blink;
        break;
    }
}