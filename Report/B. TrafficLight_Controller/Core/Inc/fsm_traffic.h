/*
 * fsm_traffic.h
 * Finite State Machine (FSM) module for traffic light control
 */

#ifndef INC_FSM_TRAFFIC_H_
#define INC_FSM_TRAFFIC_H_

#include "main.h"
#include "global.h"
#include "button.h"
#include "led_display.h"
#include "7segment_display.h"

/* ==================================================================
 * FUNCTION PROTOTYPES - FSM CONTROL
 * ================================================================== */

/**
 * @brief Initialize traffic light system
 * @details Set default values, reset all variables and LEDs
 * @note Call once only at program startup
 */
void traffic_init(void);

/**
 * @brief Main FSM function - called in timer interrupt (every 10ms)
 * @details Process logic based on current mode and update displays
 * @note Frequency: 100Hz (100 times/second)
 */
void traffic_run(void);

/**
 * @brief Update button state (edge detection)
 * @details Read button states and detect "just pressed" events
 */
void update_button_state(void);

/**
 * @brief Auto-adjust light durations to maintain constraint: RED = GREEN + AMBER
 * @param modified_light Light just modified (0=GREEN, 1=AMBER, 2=RED)
 * @return 1 if adjusted/reset, 0 if already valid
 * @details
 *   - Modified RED: Keep AMBER, calculate GREEN = RED - AMBER
 *   - Modified AMBER: Keep GREEN, calculate RED = GREEN + AMBER
 *   - Modified GREEN: Keep AMBER, calculate RED = GREEN + AMBER
 *   - If invalid: Reset to defaults (RED=5, GREEN=3, AMBER=2)
 */
int auto_adjust_duration(int modified_light);

/**
 * @brief FSM Mode 1: Normal operation (automatic traffic light)
 * @details Cycle: INIT → RED_GREEN → RED_AMBER → GREEN_RED → AMBER_RED → loop
 */
void fsm_normal_mode(void);

/**
 * @brief FSM Mode 2: RED duration adjustment
 * @details
 *   - MODE button: Switch to Mode 3
 *   - MODIFY button: Increase temp_duration
 *   - SET button: Save and auto-adjust other lights, return to Mode 1
 *   - RED LEDs blink at 2Hz
 */
void fsm_red_modify_mode(void);

/**
 * @brief FSM Mode 3: AMBER duration adjustment
 * @details
 *   - MODE button: Switch to Mode 4
 *   - MODIFY button: Increase temp_duration
 *   - SET button: Save and auto-adjust other lights, return to Mode 1
 *   - AMBER LEDs blink at 2Hz
 */
void fsm_amber_modify_mode(void);

/**
 * @brief FSM Mode 4: GREEN duration adjustment
 * @details
 *   - MODE button: Return to Mode 1
 *   - MODIFY button: Increase temp_duration
 *   - SET button: Save and auto-adjust other lights, return to Mode 1
 *   - GREEN LEDs blink at 2Hz
 */
void fsm_green_modify_mode(void);

#endif /* INC_FSM_TRAFFIC_H_ */