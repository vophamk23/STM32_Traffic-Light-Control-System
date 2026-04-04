/*
 * global.h
 * Global variables, enums, and constants for traffic light control system
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_
/* ============================================================================
 * TIMER CONFIGURATION - Exercise 4
 * ============================================================================
 * INSTRUCTION: Only modify the value of TIMER_INTERRUPT_MS to adjust
 * the timer interrupt frequency. All dependent calculations will
 * automatically update accordingly.
 *
 * EXAMPLES:
 * - TIMER_INTERRUPT_MS = 1   → Timer interrupt occurs every 1 ms
 * - TIMER_INTERRUPT_MS = 10  → Timer interrupt occurs every 10 ms (default)
 * - TIMER_INTERRUPT_MS = 100 → Timer interrupt occurs every 100 ms
 */

// CONFIGURATION PARAMETER - Change only this value
#define TIMER_INTERRUPT_MS 10 // Timer interrupt interval (in milliseconds)

// AUTO-CALCULATED CONSTANTS (DO NOT MODIFY)
#define CYCLES_PER_SECOND (1000 / TIMER_INTERRUPT_MS) // Number of interrupts per second
#define CYCLES_PER_250MS (250 / TIMER_INTERRUPT_MS)   // Number of interrupts per 250 ms

// Derived constants for system logic
#define TIMER_CYCLE CYCLES_PER_SECOND      // Used for 1-second traffic light updates
#define MAX_BLINK_COUNTER CYCLES_PER_250MS // Used for LED blinking at 2 Hz (250 ms cycle)

/* ============================================================================
 * END OF TIMER CONFIGURATION
 * ============================================================================ */

/* ==================================================================
 * ENUM DECLARATIONS
 * ================================================================== */

// System operation modes
enum MODE
{
    MODE_1_NORMAL = 1,       // Mode 1: Normal operation (automatic switching)
    MODE_2_RED_MODIFY = 2,   // Mode 2: RED duration adjustment
    MODE_3_AMBER_MODIFY = 3, // Mode 3: AMBER duration adjustment
    MODE_4_GREEN_MODIFY = 4  // Mode 4: GREEN duration adjustment
};

// Traffic light states
enum TRAFFIC_STATE
{
    INIT,      // Initial state
    RED_GREEN, // Road 1: Red – Road 2: Green
    RED_AMBER, // Road 1: Red – Road 2: Amber
    GREEN_RED, // Road 1: Green – Road 2: Red
    AMBER_RED  // Road 1: Amber – Road 2: Red
};

// Button states
enum BUTTON_STATE
{
    BTN_RELEASE, // Button released
    BTN_PRESS    // Button pressed
};

/* ==================================================================
 * GLOBAL VARIABLES (extern: defined in .c file)
 * ================================================================== */

// Light durations (in seconds)
extern int duration_RED;   // RED light duration
extern int duration_AMBER; // AMBER light duration
extern int duration_GREEN; // GREEN light duration

// Temporary variable and road counters
extern int temp_duration; // Temporary value for adjustment modes
extern int counter_road1; // Countdown timer for road 1
extern int counter_road2; // Countdown timer for road 2

// Current system state
extern enum MODE current_mode;           // Current operation mode
extern enum TRAFFIC_STATE traffic_state; // Current traffic light state

// LED blinking control
extern int blink_counter; // Blink counter
extern int flag_blink;    // Blink state flag

// LED state flags (for 2 roads)
extern int flagRed[2];    // RED LED states for both roads
extern int flagGreen[2];  // GREEN LED states for both roads
extern int flagYellow[2]; // AMBER LED states for both roads

// Button states
extern enum BUTTON_STATE prevState[3]; // Previous state of 3 buttons
extern enum BUTTON_STATE currState[3]; // Current state of 3 buttons

#endif /* INC_GLOBAL_H_ */