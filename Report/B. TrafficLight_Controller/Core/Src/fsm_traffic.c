/*
 * ============================================================================
 * fsm_traffic.c - TRAFFIC LIGHT CONTROL SYSTEM
 * ============================================================================
 *
 * OVERVIEW:
 * Traffic light controller for 2-way intersection with 4 modes:
 *
 * MODE 1: Auto operation (automatic light switching)
 * MODE 2: RED duration adjustment
 * MODE 3: AMBER duration adjustment
 * MODE 4: GREEN duration adjustment
 *
 * CONSTRAINT: duration_RED = duration_GREEN + duration_AMBER
 *
 * HARDWARE:
 * - Button 1 (MODE): Switch between modes
 * - Button 2 (MODIFY/INC): Increase duration value
 * - Button 3 (SET): Save value and return to auto mode
 * - LED: 6 LEDs for traffic lights (3 colors x 2 roads)
 * - 7-segment: Display countdown timer
 *
 * UPDATE RATE: 10ms (100Hz)
 * ============================================================================
 */

#include "fsm_traffic.h"

/* ============================================================================
 * SYSTEM INITIALIZATION
 * ============================================================================ */

/**
 * traffic_init() - Initialize the entire traffic system
 *
 * Sets default durations, mode, state, and button states
 * Called once in main() before entering main loop
 */
void traffic_init(void)
{
    // Default durations
    duration_RED = 5;   // Red light: 5 seconds
    duration_AMBER = 2; // Amber light: 2 seconds
    duration_GREEN = 3; // Green light: 3 seconds

    // Initial mode and state
    current_mode = MODE_1_NORMAL;
    traffic_state = INIT; // Will transition to RED_GREEN on first run

    // Initialize counters
    counter_road1 = 0;
    counter_road2 = 0;

    // Turn off all LEDs
    turn_off_all_leds();

    // Initialize button edge detection
    prevState[0] = BTN_RELEASE; // MODE button
    prevState[1] = BTN_RELEASE; // MODIFY button
    prevState[2] = BTN_RELEASE; // SET button

    currState[0] = BTN_RELEASE;
    currState[1] = BTN_RELEASE;
    currState[2] = BTN_RELEASE;

    // Initialize LED blinking
    // Cycle: 0.25s ON + 0.25s OFF = 0.5s
    blink_counter = 0;
    flag_blink = 0;

    // Reset LED flags
    flagRed[0] = flagRed[1] = 0;
    flagGreen[0] = flagGreen[1] = 0;
    flagYellow[0] = flagYellow[1] = 0;

    // Reset temp variable
    temp_duration = 0;
}

/* ============================================================================
 * MAIN FUNCTION - SYSTEM HEART
 * ============================================================================ */

/**
 * traffic_run() - Main function called every 10ms
 *
 * EXECUTION ORDER (IMPORTANT):
 * 1. update_button_state()   → Read button states
 * 2. fsm_*_mode()            → Process logic by current mode
 * 3. update_led_display()    → Update LED hardware
 * 4. update_7seg_display()   → Update 7-segment display
 *
 * CALL RATE: 100 times/second = 100Hz
 */
void traffic_run(void)
{
    // Step 1: Read button states
    update_button_state();

    // Step 2: Process logic by mode
    switch (current_mode)
    {
    case MODE_1_NORMAL:
        fsm_normal_mode(); // Auto mode
        break;

    case MODE_2_RED_MODIFY:
        fsm_red_modify_mode(); // Adjust RED duration
        break;

    case MODE_3_AMBER_MODIFY:
        fsm_amber_modify_mode(); // Adjust AMBER duration
        break;

    case MODE_4_GREEN_MODIFY:
        fsm_green_modify_mode(); // Adjust GREEN duration
        break;
    }

    // Step 3: Update LED hardware
    update_led_display();

    // Step 4: Update 7-segment display
    update_7seg_display();
}

/* ============================================================================
 * FSM MODE 1 - AUTO OPERATION
 * ============================================================================ */

/**
 * fsm_normal_mode() - State machine for auto operation
 *
 * TRAFFIC CYCLE (4 states):
 *   INIT → RED_GREEN → RED_AMBER → GREEN_RED → AMBER_RED → (loop)
 *
 * Updates every 1 second (counts down counter_road1/2)
 */
void fsm_normal_mode(void)
{
    static int timer_counter = 0;

    // Handle MODE button - switch to adjustment mode
    if (currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE)
    {
        current_mode = MODE_2_RED_MODIFY;
        temp_duration = duration_RED;
        turn_off_all_leds();
        return;
    }

    // Count timer cycles
    timer_counter++;
    if (timer_counter < TIMER_CYCLE)
    {
        return; // Not enough time yet
    }
    timer_counter = 0; // Reset for next cycle

    // Traffic FSM - updates every second
    switch (traffic_state)
    {
    case INIT:
        // First state initialization
        traffic_state = RED_GREEN;
        counter_road1 = duration_RED;
        counter_road2 = duration_GREEN;
        break;

    case RED_GREEN:
        // Road 1: RED, Road 2: GREEN
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
        // Road 1: RED, Road 2: AMBER
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
        // Road 1: GREEN, Road 2: RED
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
        // Road 1: AMBER, Road 2: RED
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

    // Prevent negative counters
    if (counter_road1 < 0)
        counter_road1 = 0;
    if (counter_road2 < 0)
        counter_road2 = 0;
}

/* ============================================================================
 * FSM MODE 2 - RED DURATION ADJUSTMENT
 * ============================================================================ */

/**
 * fsm_red_modify_mode() - Adjust RED light duration
 *
 * - Blinks RED LEDs
 * - MODE button: Switch to MODE 3 (AMBER adjust)
 * - MODIFY button: Increase temp_duration (1→99→1)
 * - SET button: Save and auto-adjust other durations
 */
void fsm_red_modify_mode(void)
{
    // MODE button - switch to AMBER adjust
    if (currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE)
    {
        current_mode = MODE_3_AMBER_MODIFY;
        temp_duration = duration_AMBER;
        return;
    }

    // MODIFY button - increase value
    if (currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE)
    {
        temp_duration++;
        if (temp_duration > 99)
        {
            temp_duration = 1;
        }
    }

    // SET button - save and auto-adjust
    if (currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE)
    {
        duration_RED = temp_duration;
        auto_adjust_duration(0); // 0 = RED was modified
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    // Blink RED LEDs
    handle_led_blinking(0); // 0 = RED
}

/* ============================================================================
 * FSM MODE 3 - AMBER DURATION ADJUSTMENT
 * ============================================================================ */

/**
 * fsm_amber_modify_mode() - Adjust AMBER light duration
 *
 * - Blinks AMBER LEDs
 * - MODE button: Switch to MODE 4 (GREEN adjust)
 * - MODIFY button: Increase temp_duration
 * - SET button: Save and auto-adjust
 */
void fsm_amber_modify_mode(void)
{
    // MODE button - switch to GREEN adjust
    if (currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE)
    {
        current_mode = MODE_4_GREEN_MODIFY;
        temp_duration = duration_GREEN;
        return;
    }

    // MODIFY button - increase value
    if (currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE)
    {
        temp_duration++;
        if (temp_duration > 99)
            temp_duration = 1;
    }

    // SET button - save and auto-adjust
    if (currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE)
    {
        duration_AMBER = temp_duration;
        auto_adjust_duration(1); // 1 = AMBER was modified
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    // Blink AMBER LEDs
    handle_led_blinking(1); // 1 = AMBER
}

/* ============================================================================
 * FSM MODE 4 - GREEN DURATION ADJUSTMENT
 * ============================================================================ */

/**
 * fsm_green_modify_mode() - Adjust GREEN light duration
 *
 * - Blinks GREEN LEDs
 * - MODE button: Return to MODE 1 (no save)
 * - MODIFY button: Increase temp_duration
 * - SET button: Save and auto-adjust
 */
void fsm_green_modify_mode(void)
{
    // MODE button - return to auto mode (no save)
    if (currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE)
    {
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    // MODIFY button - increase value
    if (currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE)
    {
        temp_duration++;
        if (temp_duration > 99)
            temp_duration = 1;
    }

    // SET button - save and auto-adjust
    if (currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE)
    {
        duration_GREEN = temp_duration;
        auto_adjust_duration(2); // 2 = GREEN was modified
        current_mode = MODE_1_NORMAL;
        traffic_state = INIT;
        turn_off_all_leds();
        return;
    }

    // Blink GREEN LEDs
    handle_led_blinking(2); // 2 = GREEN
}

/* ============================================================================
 * AUTO-ADJUST DURATION FUNCTION
 * ============================================================================ */

/**
 * auto_adjust_duration() - Auto-adjust other durations to maintain constraint
 *
 * CONSTRAINT: duration_RED = duration_GREEN + duration_AMBER
 *
 * STRATEGY:
 * - Modified RED (0): Keep AMBER, calculate GREEN = RED - AMBER
 * - Modified AMBER (1): Update GREEN = AMBER + 4, calculate RED = GREEN + AMBER
 * - Modified GREEN (2): Keep AMBER, calculate RED = GREEN + AMBER
 *
 * PARAMETERS:
 *   modified_light: Which light was modified
 *                   0 = GREEN, 1 = AMBER, 2 = RED
 *
 * RETURN:
 *   1: Adjusted or reset
 *   0: No adjustment needed (already valid)
 *
 * DEFAULT ON RESET: RED=5, GREEN=3, AMBER=2
 */
int auto_adjust_duration(int modified_light)
{
    // Check if constraint is already satisfied
    if (duration_RED == (duration_GREEN + duration_AMBER))
    {
        return 0; // No adjustment needed
    }

    switch (modified_light)
    {
    case 0: // RED was modified
        // Strategy: Keep AMBER, calculate GREEN
        duration_GREEN = duration_RED - duration_AMBER;

        // Validate GREEN
        if (duration_GREEN < 1 || duration_GREEN > 99)
        {
            duration_GREEN = duration_RED - duration_AMBER;
            duration_AMBER = duration_RED - duration_GREEN;

            // Validate AMBER
            if (duration_AMBER < 1 || duration_AMBER > 99)
            {
                // Reset to defaults
                duration_RED = 5;
                duration_GREEN = 3;
                duration_AMBER = 2;
            }
        }
        break;

    case 1: // AMBER was modified
        // Strategy: GREEN = AMBER + 4, RED = GREEN + AMBER
        duration_GREEN = duration_AMBER + 4;
        duration_RED = duration_GREEN + duration_AMBER;

        // Check if RED exceeds limit
        if (duration_RED > 99)
        {
            // Adjust to fit within limits
            duration_AMBER = (99 - 3) / 2;       // = 48
            duration_GREEN = duration_AMBER + 3; // = 51
            duration_RED = 99;

            if (duration_AMBER < 1)
            {
                // Reset if invalid
                duration_RED = 5;
                duration_GREEN = 3;
                duration_AMBER = 2;
            }
        }

        // Validate GREEN
        if (duration_GREEN < 1 || duration_GREEN > 99)
        {
            duration_RED = 5;
            duration_GREEN = 3;
            duration_AMBER = 2;
        }
        break;

    case 2: // GREEN was modified
        // Strategy: Keep AMBER, calculate RED
        duration_RED = duration_GREEN + duration_AMBER;

        // Check if RED exceeds limit
        if (duration_RED > 99)
        {
            // Reduce AMBER to fit
            duration_AMBER = 99 - duration_GREEN;
            duration_RED = 99;

            // Validate AMBER
            if (duration_AMBER < 1)
            {
                // Reset if invalid
                duration_RED = 5;
                duration_GREEN = 3;
                duration_AMBER = 2;
            }
        }
        break;
    }

    return 1; // Adjustment completed
}

/* ============================================================================
 * BUTTON EDGE DETECTION
 * ============================================================================ */

/**
 * update_button_state() - Update button states and detect events
 *
 * EDGE DETECTION:
 * Detects rising edge (button just pressed):
 *   prevState == RELEASE && currState == PRESS → PROCESS EVENT
 *
 * PREVENTS:
 * - Multiple triggers while holding button
 * - Each press = exactly one event
 *
 * CALLED: Every 10ms in traffic_run()
 */
void update_button_state(void)
{
    for (int i = 0; i < 3; i++)
    {
        // Save current state as previous
        prevState[i] = currState[i];

        // Read new state from hardware
        switch (i)
        {
        case 0: // MODE button
            if (isButton1Pressed())
            {
                currState[i] = BTN_PRESS;
            }
            else
            {
                currState[i] = BTN_RELEASE;
            }
            break;

        case 1: // MODIFY button
            if (isButton2Pressed())
            {
                currState[i] = BTN_PRESS;
            }
            else
            {
                currState[i] = BTN_RELEASE;
            }
            break;

        case 2: // SET button
            if (isButton3Pressed())
            {
                currState[i] = BTN_PRESS;
            }
            else
            {
                currState[i] = BTN_RELEASE;
            }
            break;
        }
    }
}

/* ============================================================================
 * SYSTEM DIAGRAMS
 * ============================================================================ */

/*
 * MODE TRANSITION:
 *
 *    MODE 1 (Auto) ──[MODE btn]──> MODE 2 (RED adjust)
 *                                         │
 *                                   [MODE btn]
 *                                         │
 *                                         ↓
 *                                   MODE 3 (AMBER adjust)
 *                                         │
 *                                   [MODE btn]
 *                                         │
 *                                         ↓
 *                                   MODE 4 (GREEN adjust)
 *                                         │
 *                            [MODE or SET btn]
 *                                         │
 *                                         ↓
 *    MODE 1 (Auto) <────────────────────┘
 *
 *
 * TRAFFIC CYCLE (MODE 1):
 *
 *    INIT → RED_GREEN → RED_AMBER → GREEN_RED → AMBER_RED → (loop)
 *           Road1: RED   Road1: RED   Road1: GRN  Road1: AMB
 *           Road2: GRN   Road2: AMB   Road2: RED  Road2: RED
 *
 *
 * EDGE DETECTION TIMELINE:
 *
 *    Time   │ Physical │ prevState │ currState │ Detected │ Action
 *    ───────┼──────────┼───────────┼───────────┼──────────┼────────
 *    t=0ms  │  OFF     │  RELEASE  │  RELEASE  │    -     │   -
 *    t=10ms │  OFF     │  RELEASE  │  RELEASE  │    -     │   -
 *    t=20ms │  PRESS   │  RELEASE  │  PRESS    │ RISING ✓ │ PROCESS
 *    t=30ms │  HOLD    │  PRESS    │  PRESS    │    -     │ Ignore
 *    t=40ms │  HOLD    │  PRESS    │  PRESS    │    -     │ Ignore
 *    t=50ms │  RELEASE │  PRESS    │  RELEASE  │ FALLING  │ Ignore
 */