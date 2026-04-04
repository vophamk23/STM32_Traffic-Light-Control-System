/*
 * seven_segment.c
 * BCD (Binary Coded Decimal) 7-segment LED display module
 * Each decimal digit is encoded using 4 binary bits
 */
#include "7segment_display.h"

/**
 * update_7seg_display() - Update all 7-segment displays
 *
 * Display logic:
 * - MODE 1: Show countdown timers for both roads
 * - MODE 2-4: Show temp_duration being adjusted
 */
void update_7seg_display(void)
{
    // Normal operation mode
    if (current_mode == MODE_1_NORMAL)
    {
        display_7seg_left(counter_road1);  // Left: road 1 timer
        display_7seg_right(counter_road2); // Right: road 2 timer
        display_7seg_mode(1);              // Display mode number = 1
    }
    // Adjustment modes (2, 3, 4)
    else
    {
        // Both displays show the value being adjusted
        display_7seg_left(temp_duration);  // Left: temp value
        display_7seg_right(temp_duration); // Right: temp value (same)
        display_7seg_mode(current_mode);   // Display current mode (2, 3, or 4)
    }
}

/* ==================================================================
 * BCD 7-SEGMENT DISPLAY FUNCTIONS
 * ================================================================== */

/**
 * display_7seg_left() - Display 2-digit number on LEFT 7-segment pair
 * @param num: Number to display (0-99)
 *
 * Mechanism:
 * - Split number into tens and units
 * - Encode each digit into 4-bit BCD
 * - Output to corresponding GPIO pins
 */
void display_7seg_left(int num)
{
    // Split into 2 digits
    int tens = num / 10;  // Tens digit (e.g., 45 -> 4)
    int units = num % 10; // Units digit (e.g., 45 -> 5)

    // SEG0 - Display TENS digit using PA12-PA15
    HAL_GPIO_WritePin(GPIOA, inputseg0_0_Pin, (tens & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 0 (LSB)
    HAL_GPIO_WritePin(GPIOA, inputseg0_1_Pin, (tens & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 1
    HAL_GPIO_WritePin(GPIOA, inputseg0_2_Pin, (tens & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 2
    HAL_GPIO_WritePin(GPIOA, inputseg0_3_Pin, (tens & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 3 (MSB)

    // SEG1 - Display UNITS digit using PB0-PB3
    HAL_GPIO_WritePin(GPIOB, inputseg1_0_Pin, (units & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 0
    HAL_GPIO_WritePin(GPIOB, inputseg1_1_Pin, (units & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 1
    HAL_GPIO_WritePin(GPIOB, inputseg1_2_Pin, (units & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 2
    HAL_GPIO_WritePin(GPIOB, inputseg1_3_Pin, (units & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 3
}

/**
 * display_7seg_right() - Display 2-digit number on RIGHT 7-segment pair
 * @param num: Number to display (0-99)
 *
 * Similar to display_7seg_left but uses different GPIO pins
 */
void display_7seg_right(int num)
{
    // Split into 2 digits
    int tens = num / 10;  // Tens digit
    int units = num % 10; // Units digit

    // SEG2 - Display TENS digit using PB4-PB7
    HAL_GPIO_WritePin(GPIOB, inputseg2_0_Pin, (tens & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg2_1_Pin, (tens & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg2_2_Pin, (tens & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg2_3_Pin, (tens & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // SEG3 - Display UNITS digit using PB8-PB11
    HAL_GPIO_WritePin(GPIOB, inputseg3_0_Pin, (units & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg3_1_Pin, (units & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg3_2_Pin, (units & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, inputseg3_3_Pin, (units & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * display_7seg_mode() - Display current MODE number
 * @param mode: Mode number to display (0-15, using 4 bits)
 *
 * Shows current system operation mode:
 * - Mode 1: Normal operation
 * - Mode 2: RED adjustment
 * - Mode 3: AMBER adjustment
 * - Mode 4: GREEN adjustment
 */
void display_7seg_mode(int mode)
{
    // Display mode using PB12-PB15
    HAL_GPIO_WritePin(GPIOB, inputmode_0_Pin, (mode & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 0
    HAL_GPIO_WritePin(GPIOB, inputmode_1_Pin, (mode & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 1
    HAL_GPIO_WritePin(GPIOB, inputmode_2_Pin, (mode & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 2
    HAL_GPIO_WritePin(GPIOB, inputmode_3_Pin, (mode & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Bit 3
}