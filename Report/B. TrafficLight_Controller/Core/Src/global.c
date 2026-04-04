/*
 * global.c
 * Global variables for traffic light control system
 * Contains all variables shared between different modules
 */
#include "global.h"

/* ==================================================================
 * GLOBAL VARIABLES
 * ================================================================== */

// ============ TRAFFIC LIGHT DURATIONS ============
// Duration in seconds for each light state
// Can be adjusted through MODE 2, 3, 4

int duration_RED = 5;   // RED light duration: 5 seconds
int duration_AMBER = 2; // AMBER light duration: 2 seconds
int duration_GREEN = 3; // GREEN light duration: 3 seconds

// CONSTRAINT: duration_RED = duration_GREEN + duration_AMBER
// Example: RED(5) = GREEN(3) + AMBER(2)

// ============ TEMPORARY VARIABLES AND COUNTERS ============

int temp_duration = 0; // Temporary value during adjustment (Mode 2/3/4)
                       // Only saved to duration_* when SET button pressed

int counter_road1 = 0; // Countdown timer for ROAD 1
                       // Displayed on left 7-segment

int counter_road2 = 0; // Countdown timer for ROAD 2
                       // Displayed on right 7-segment

// ============ SYSTEM STATE ============

enum MODE current_mode = MODE_1_NORMAL;
// Current operation mode:
// - MODE_1_NORMAL: Normal operation
// - MODE_2_RED_MODIFY: RED duration adjustment
// - MODE_3_AMBER_MODIFY: AMBER duration adjustment
// - MODE_4_GREEN_MODIFY: GREEN duration adjustment

enum TRAFFIC_STATE traffic_state = INIT;
// Current traffic light state:
// - INIT: Initial state
// - RED_GREEN: Road 1 red, Road 2 green
// - RED_AMBER: Road 1 red, Road 2 amber
// - GREEN_RED: Road 1 green, Road 2 red
// - AMBER_RED: Road 1 amber, Road 2 red

// ============ LED BLINKING CONTROL ============

int blink_counter = 0; // Counter for blinking effect
                       // Increments every timer interrupt (10ms)
                       // Resets when reaching MAX_BLINK_COUNTER

int flag_blink = 0; // Blink state flag
                    // 0: LED off in blink cycle
                    // 1: LED on in blink cycle
                    // Toggles when blink_counter reaches MAX

// ============ LED STATE FLAGS ============
// Arrays store state for each light type on 2 roads
// Index [0] = Road 1, Index [1] = Road 2
// Value: 0 = off, 1 = on

int flagRed[2] = {0, 0};    // RED LED flags
int flagGreen[2] = {0, 0};  // GREEN LED flags
int flagYellow[2] = {0, 0}; // AMBER LED flags

// ============ BUTTON STATE ============
// Store state of 3 buttons: MODE, MODIFY, SET
// Used for button edge detection

enum BUTTON_STATE prevState[3] = {BTN_RELEASE, BTN_RELEASE, BTN_RELEASE};
// Previous state of 3 buttons
// Index [0]: MODE button
// Index [1]: MODIFY button
// Index [2]: SET button

enum BUTTON_STATE currState[3] = {BTN_RELEASE, BTN_RELEASE, BTN_RELEASE};
// Current state of 3 buttons
// BTN_RELEASE: Button not pressed
// BTN_PRESS: Button pressed
// Compare with prevState to detect "just pressed" event
// (prevState = RELEASE, currState = PRESS)