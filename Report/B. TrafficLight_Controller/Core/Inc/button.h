/*
 * button.h
 *  Created on: Oct 5, 2023
 *      Author: KAI
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

// Button states (Pull-up mode: active-low)
#define NORMAL_STATE SET    // Button released (GPIO = 1)
#define PRESSED_STATE RESET // Button pressed (GPIO = 0)

// Flags for 3 buttons: [0]=Button1, [1]=Button2, [2]=Button3
extern int button_flag[3];         // Short press flag
extern int button_long_pressed[3]; // Long press flag (>500ms)

// Short press check (returns 1 once per press)
int isButton1Pressed(void);
int isButton2Pressed(void);
int isButton3Pressed(void);

// Long press check (returns 1 once, repeats every 500ms while held)
int isButton1LongPressed(void);
int isButton2LongPressed(void);
int isButton3LongPressed(void);

// Button processing function - CALL IN TIMER INTERRUPT EVERY 1ms
void getKeyInput(void);

/*
 * USAGE GUIDE:
 * 1. Call getKeyInput() in the Timer Interrupt every 1ms.
 * 2. In the main loop:
 *      if (isButton1Pressed()) { ... }      // Short press
 *      if (isButton1LongPressed()) { ... }  // Long press
 */

#endif /* INC_BUTTON_H_ */
