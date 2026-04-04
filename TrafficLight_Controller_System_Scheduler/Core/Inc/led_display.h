/*
 * led_display.h
 * Module xử lý LED đơn (đèn giao thông)
 */

#ifndef INC_LED_DISPLAY_H_
#define INC_LED_DISPLAY_H_

#include "main.h"
#include "global.h"

/* ==================================================================
 * FUNCTION PROTOTYPES - LED CONTROL
 * ================================================================== */

/**
 * @brief Tắt tất cả LED giao thông
 */
void turn_off_all_leds(void);

/**
 * @brief Điều khiển đèn giao thông cho một đường
 * @param road: 0 = Đường 1, 1 = Đường 2
 * @param red: 1 = Bật đèn đỏ, 0 = Tắt
 * @param amber: 1 = Bật đèn vàng, 0 = Tắt
 * @param green: 1 = Bật đèn xanh, 0 = Tắt
 */
void set_traffic_led(int road, int red, int amber, int green);

/**
 * @brief Hiển thị LED đỏ
 * @param IS_ON: 0 = SÁNG (do active LOW), 1 = TẮT
 * @param index: 0 = Đường 1, 1 = Đường 2
 */
void displayLED_RED(int IS_ON, int index);

/**
 * @brief Hiển thị LED vàng
 * @param IS_ON: 0 = SÁNG (do active LOW), 1 = TẮT
 * @param index: 0 = Đường 1, 1 = Đường 2
 */
void displayLED_YELLOW(int IS_ON, int index);

/**
 * @brief Hiển thị LED xanh
 * @param IS_ON: 0 = SÁNG (do active LOW), 1 = TẮT
 * @param index: 0 = Đường 1, 1 = Đường 2
 */
void displayLED_GREEN(int IS_ON, int index);

/**
 * @brief Xử lý LED nhấp nháy (2Hz)
 * @param led_type: 0 = GREEN, 1 = YELLOW, 2 = RED
 */
void handle_led_blinking(int led_type);

/**
 * @brief Cập nhật hiển thị LED theo mode hiện tại
 */
void update_led_display(void);

#endif /* INC_LED_DISPLAY_H_ */
