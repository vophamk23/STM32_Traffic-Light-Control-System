/*
 * 7seg_display.h
 * Header file for BCD 7-segment LED control module
 *
 * System description:
 * - 4 × 7-segment LEDs: SEG0, SEG1, SEG2, SEG3
 * - 1 × mode display: shows operation mode (1-4)
 * - Each 7-segment controlled by 4-bit BCD (0-9)
 *
 * Distribution:
 * - SEG0 + SEG1: LEFT display (road 1 or temp_duration)
 * - SEG2 + SEG3: RIGHT display (road 2 or temp_duration)
 * - MODE: Display mode number (1, 2, 3, 4)
 *
 * Created on: Oct 13, 2025
 * Author: ASUS
 */

#ifndef INC_7SEGMENT_DISPLAY_H_
#define INC_7SEGMENT_DISPLAY_H_

// ============ INCLUDES ============
#include "main.h"   // HAL definitions, GPIO pins
#include "global.h" // Global variables (counter, mode, duration...)

/* ==================================================================
 * FUNCTION PROTOTYPES
 * ================================================================== */

/**
 * @brief Display 2-digit number on LEFT 7-segment pair
 *
 * @param num: Number to display (0-99)
 *
 * Details:
 * - Splits number into tens and units
 * - SEG0 (PA12-PA15): Display TENS digit
 * - SEG1 (PB0-PB3): Display UNITS digit
 * - Uses BCD encoding (4 bits per digit)
 *
 * Example:
 * - display_7seg_left(45) → SEG0 shows "4", SEG1 shows "5"
 * - display_7seg_left(7) → SEG0 shows "0", SEG1 shows "7"
 *
 * Usage:
 * - Normal mode: Display counter_road1 (road 1 countdown)
 * - Adjust mode: Display temp_duration (value being adjusted)
 */
void display_7seg_left(int num);

/**
 * @brief Display 2-digit number on RIGHT 7-segment pair
 *
 * @param num: Number to display (0-99)
 *
 * Details:
 * - SEG2 (PB4-PB7): Display TENS digit
 * - SEG3 (PB8-PB11): Display UNITS digit
 * - Similar to display_7seg_left() but uses different GPIO pins
 *
 * Example:
 * - display_7seg_right(23) → SEG2 shows "2", SEG3 shows "3"
 *
 * Usage:
 * - Normal mode: Display counter_road2 (road 2 countdown)
 * - Adjust mode: Display temp_duration (same as left)
 */
void display_7seg_right(int num);

/**
 * @brief Display current MODE number on mode 7-segment
 *
 * @param mode: Mode number to display (1-4, or 0-15 with 4-bit BCD)
 *
 * Details:
 * - Uses 4 GPIO pins PB12-PB15
 * - Displays mode using BCD encoding
 * - Shows user which mode system is in
 *
 * Mode meanings:
 * - Mode 1: Normal operation (automatic traffic light)
 * - Mode 2: RED duration adjustment
 * - Mode 3: AMBER duration adjustment
 * - Mode 4: GREEN duration adjustment
 *
 * Example:
 * - display_7seg_mode(1) → Display "1" (normal mode)
 * - display_7seg_mode(2) → Display "2" (red modify mode)
 */
void display_7seg_mode(int mode);

/**
 * @brief Main function to update ALL 7-segment displays based on current mode
 *
 * @param None (reads from global variables)
 *
 * Logic:
 *
 * ┌─────────────────────────────────────────────────────────────┐
 * │ if (current_mode == MODE_1_NORMAL)                          │
 * │    ├─ LEFT display: counter_road1 (road 1 time)            │
 * │    ├─ RIGHT display: counter_road2 (road 2 time)           │
 * │    └─ MODE display: 1                                       │
 * │                                                             │
 * │ else (MODE 2/3/4 - Adjustment mode)                        │
 * │    ├─ LEFT display: temp_duration (value being adjusted)   │
 * │    ├─ RIGHT display: temp_duration (same as left)          │
 * │    └─ MODE display: 2, 3, or 4                             │
 * └─────────────────────────────────────────────────────────────┘
 *
 * Mechanism:
 * - Called in traffic_run() every 10ms
 * - Automatically selects display data based on current_mode
 * - Ensures display is synchronized with system state
 *
 * Example displays:
 *
 * MODE 1 (Normal):
 * ┌──────────────────────┐
 * │ [05] [03]      [1]   │  ← Road 1: 5s, Road 2: 3s, Mode: 1
 * └──────────────────────┘
 *
 * MODE 2 (Red adjustment):
 * ┌──────────────────────┐
 * │ [08] [08]      [2]   │  ← Adjusting value 8s, Mode: 2
 * └──────────────────────┘
 *
 * Note:
 * - This function ONLY UPDATES display, does not change counter values
 * - Counter decrement is done in fsm_normal_mode()
 */
void update_7seg_display(void);

#endif /* INC_7SEGMENT_DISPLAY_H_ */