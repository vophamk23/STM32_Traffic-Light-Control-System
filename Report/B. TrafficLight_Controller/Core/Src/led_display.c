/*
 * led_display.c
 * LED control module for traffic light system
 * Controls 6 LEDs: RED1, YELLOW1, GREEN1, RED2, YELLOW2, GREEN2
 */

#include "led_display.h"

/* ==================================================================
 * LED DISPLAY UPDATE
 * ================================================================== */

/**
 * update_led_display() - Update LED display based on current mode
 *
 * Logic:
 * - MODE_1_NORMAL: Display traffic lights based on traffic_state
 * - MODE 2/3/4: Display blinking LEDs based on flags
 *
 * Called continuously in main loop or from timer
 */
void update_led_display(void)
{
    // Normal operation mode
    if (current_mode == MODE_1_NORMAL)
    {
        // Display traffic lights based on FSM state
        switch (traffic_state)
        {
        case INIT:
            turn_off_all_leds();
            break;

        case RED_GREEN:                  // Road 1: RED, Road 2: GREEN
            set_traffic_led(0, 1, 0, 0); // Road 1: only red
            set_traffic_led(1, 0, 0, 1); // Road 2: only green
            break;

        case RED_AMBER:                  // Road 1: RED, Road 2: AMBER
            set_traffic_led(0, 1, 0, 0); // Road 1: only red
            set_traffic_led(1, 0, 1, 0); // Road 2: only amber
            break;

        case GREEN_RED:                  // Road 1: GREEN, Road 2: RED
            set_traffic_led(0, 0, 0, 1); // Road 1: only green
            set_traffic_led(1, 1, 0, 0); // Road 2: only red
            break;

        case AMBER_RED:                  // Road 1: AMBER, Road 2: RED
            set_traffic_led(0, 0, 1, 0); // Road 1: only amber
            set_traffic_led(1, 1, 0, 0); // Road 2: only red
            break;
        }
    }
    // Adjustment modes (MODE 2/3/4)
    else
    {
        // Display LEDs based on flags updated by handle_led_blinking()
        // Flags toggle every 500ms to create blinking effect

        // Update red LEDs for both roads
        displayLED_RED(flagRed[0], 0);
        displayLED_RED(flagRed[1], 1);

        // Update amber LEDs for both roads
        displayLED_YELLOW(flagYellow[0], 0);
        displayLED_YELLOW(flagYellow[1], 1);

        // Update green LEDs for both roads
        displayLED_GREEN(flagGreen[0], 0);
        displayLED_GREEN(flagGreen[1], 1);
    }
}

/* ==================================================================
 * BASIC LED CONTROL
 * ================================================================== */

/**
 * turn_off_all_leds() - Turn off all traffic LEDs
 *
 * Turns off all 6 LEDs (3 per road)
 * Used during initialization or state transitions
 */
void turn_off_all_leds(void)
{
    // Road 1 LEDs
    HAL_GPIO_WritePin(GPIOA, RED1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, GPIO_PIN_RESET);

    // Road 2 LEDs
    HAL_GPIO_WritePin(GPIOA, RED2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, GPIO_PIN_RESET);
}

/**
 * set_traffic_led() - Set traffic light state for one road
 *
 * @param road: Road index (0 = Road 1, 1 = Road 2)
 * @param red: Red light state (1 = on, 0 = off)
 * @param amber: Amber light state (1 = on, 0 = off)
 * @param green: Green light state (1 = on, 0 = off)
 *
 * NOTE: Inverted logic (active LOW)
 * - Parameter = 1 (on) → GPIO_PIN_RESET
 * - Parameter = 0 (off) → GPIO_PIN_SET
 */
void set_traffic_led(int road, int red, int amber, int green)
{
    if (road == 0)
    { // Road 1
        HAL_GPIO_WritePin(GPIOA, RED1_Pin, red ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, amber ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, green ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }
    else
    { // Road 2
        HAL_GPIO_WritePin(GPIOA, RED2_Pin, red ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, amber ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, green ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }
}

/**
 * displayLED_RED() - Control RED LED for specific road
 *
 * @param IS_ON: Desired state (1 = on, 0 = off)
 * @param index: Road index (0 = Road 1, 1 = Road 2)
 *
 * Used mainly in blinking mode
 */
void displayLED_RED(int IS_ON, int index)
{
    switch (index)
    {
    case 0: // Road 1
        HAL_GPIO_WritePin(GPIOA, RED1_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;

    case 1: // Road 2
        HAL_GPIO_WritePin(GPIOA, RED2_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    }
}

/**
 * displayLED_YELLOW() - Control AMBER LED for specific road
 *
 * @param IS_ON: Desired state (1 = on, 0 = off)
 * @param index: Road index (0 = Road 1, 1 = Road 2)
 */
void displayLED_YELLOW(int IS_ON, int index)
{
    switch (index)
    {
    case 0: // Road 1
        HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;

    case 1: // Road 2
        HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    }
}

/**
 * displayLED_GREEN() - Control GREEN LED for specific road
 *
 * @param IS_ON: Desired state (1 = on, 0 = off)
 * @param index: Road index (0 = Road 1, 1 = Road 2)
 */
void displayLED_GREEN(int IS_ON, int index)
{
    switch (index)
    {
    case 0: // Road 1
        HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;

    case 1: // Road 2
        HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    }
}

/* ==================================================================
 * LED BLINKING - 2Hz = 500ms ON/OFF CYCLE
 * ================================================================== */

/**
 * handle_led_blinking() - Handle LED blinking effect in adjustment mode
 *
 * @param led_type: LED type to blink
 *                  0 = RED (Mode 2)
 *                  1 = AMBER (Mode 3)
 *                  2 = GREEN (Mode 4)
 *
 * Mechanism:
 * - Called every 10ms from timer interrupt
 * - After 50 calls (500ms) → toggle LED state
 * - Cycle: 500ms ON + 500ms OFF = 1 second (1Hz)
 * - Only the LED being adjusted blinks, others are OFF
 */
void handle_led_blinking(int led_type)
{
    // Increment blink counter
    blink_counter++;

    // Check if enough time has passed (50 x 10ms = 500ms)
    if (blink_counter >= MAX_BLINK_COUNTER)
    {
        // Reset counter for next cycle
        blink_counter = 0;

        // Toggle blink flag: 0→1 or 1→0
        // flag_blink = 0: LED off in this cycle
        // flag_blink = 1: LED on in this cycle
        flag_blink = !flag_blink;

        // Step 1: Turn off all LEDs
        // Set all flags = 1 (off) due to active LOW
        // Ensures only the LED being adjusted can light up
        flagRed[0] = 1;
        flagRed[1] = 1;
        flagGreen[0] = 1;
        flagGreen[1] = 1;
        flagYellow[0] = 1;
        flagYellow[1] = 1;

        // Step 2: Enable only the LED being adjusted
        switch (led_type)
        {
        case 0: // MODE 2: RED duration adjustment
            // Only red LEDs on BOTH roads blink simultaneously
            flagRed[0] = flag_blink;
            flagRed[1] = flag_blink;
            break;

        case 1: // MODE 3: AMBER duration adjustment
            // Only amber LEDs on BOTH roads blink simultaneously
            flagYellow[0] = flag_blink;
            flagYellow[1] = flag_blink;
            break;

        case 2: // MODE 4: GREEN duration adjustment
            // Only green LEDs on BOTH roads blink simultaneously
            flagGreen[0] = flag_blink;
            flagGreen[1] = flag_blink;
            break;
        }
    }
    // If not enough time yet, just increment counter
}