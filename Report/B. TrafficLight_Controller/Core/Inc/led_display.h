/*
 * led_display.h
 * LED control module (traffic lights)
 */

#ifndef INC_LED_DISPLAY_H_
#define INC_LED_DISPLAY_H_

#include "main.h"
#include "global.h"

/* ==================================================================
 * FUNCTION PROTOTYPES - LED CONTROL
 * ================================================================== */

/**
 * @brief Turn off all traffic LEDs
 */
void turn_off_all_leds(void);

/**
 * @brief Control traffic lights for one road
 * @param road: 0 = Road 1, 1 = Road 2
 * @param red: 1 = Red ON, 0 = OFF
 * @param amber: 1 = Amber ON, 0 = OFF
 * @param green: 1 = Green ON, 0 = OFF
 */
void set_traffic_led(int road, int red, int amber, int green);

/**
 * @brief Control RED LED
 * @param IS_ON: 1 = ON, 0 = OFF
 * @param index: 0 = Road 1, 1 = Road 2
 */
void displayLED_RED(int IS_ON, int index);

/**
 * @brief Control AMBER LED
 * @param IS_ON: 1 = ON, 0 = OFF
 * @param index: 0 = Road 1, 1 = Road 2
 */
void displayLED_YELLOW(int IS_ON, int index);

/**
 * @brief Control GREEN LED
 * @param IS_ON: 1 = ON, 0 = OFF
 * @param index: 0 = Road 1, 1 = Road 2
 */
void displayLED_GREEN(int IS_ON, int index);

/**
 * @brief Handle LED blinking effect (2Hz)
 * @param led_type: 0 = RED, 1 = AMBER, 2 = GREEN
 */
void handle_led_blinking(int led_type);

/**
 * @brief Update LED display based on current mode
 */
void update_led_display(void);

#endif /* INC_LED_DISPLAY_H_ */