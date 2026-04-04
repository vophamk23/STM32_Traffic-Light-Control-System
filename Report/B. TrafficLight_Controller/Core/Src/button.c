#include "button.h"

/* ==================================================================
 * MODULE OVERVIEW:
 *
 * This module handles 3 buttons with the following features:
 * 1. DEBOUNCING: Eliminates mechanical noise when pressing a button.
 * 2. SHORT PRESS: Detects a single short press event.
 * 3. LONG PRESS: Detects a press held longer than 1 second.
 *
 * TIMING:
 * - getKeyInput() is called every 10ms in the timer interrupt.
 * - Debounce time  = 3 × 10ms = 30ms
 * - Long press     = 100 × 10ms = 1000ms (1 second)
 * ================================================================== */

// ==================================================================
// GLOBAL VARIABLES
// ==================================================================

// Button state registers (store last 4 readings for 3 buttons)
int KeyReg0[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
int KeyReg1[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
int KeyReg2[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
int KeyReg3[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};

// Timer counters for long press detection
// 100 calls × 10ms = 1000ms (1 second)
int TimeOutForKeyPress[3] = {100, 100, 100};

// Flags for short press events
int button_flag[3] = {0, 0, 0};

// Flags for long press events
int button_long_pressed[3] = {0, 0, 0};

// Startup counter – ignore button inputs for the first 100ms
int startup_counter = 10; // 10 × 10ms = 100ms

/* ==================================================================
 * BUTTON STATE CHECK FUNCTIONS (CALLED IN MAIN LOOP)
 * ================================================================== */

int isButton1Pressed()
{
  if (button_flag[0] == 1)
  {
    button_flag[0] = 0;
    return 1;
  }
  return 0;
}

int isButton2Pressed()
{
  if (button_flag[1] == 1)
  {
    button_flag[1] = 0;
    return 1;
  }
  return 0;
}

int isButton3Pressed()
{
  if (button_flag[2] == 1)
  {
    button_flag[2] = 0;
    return 1;
  }
  return 0;
}

int isButton1LongPressed()
{
  if (button_long_pressed[0] == 1)
  {
    button_long_pressed[0] = 0;
    return 1;
  }
  return 0;
}

int isButton2LongPressed()
{
  if (button_long_pressed[1] == 1)
  {
    button_long_pressed[1] = 0;
    return 1;
  }
  return 0;
}

int isButton3LongPressed()
{
  if (button_long_pressed[2] == 1)
  {
    button_long_pressed[2] = 0;
    return 1;
  }
  return 0;
}

/* ==================================================================
 * EVENT HANDLER
 * ================================================================== */

/**
 * subKeyProcess() - Triggered when a button press is detected.
 * @index: index of the button (0–2)
 */
void subKeyProcess(int index)
{
  button_flag[index] = 1;
}

/* ==================================================================
 * MAIN BUTTON PROCESSING FUNCTION
 * ==================================================================
 *
 * getKeyInput() should be called every 10ms in the timer interrupt.
 * Steps:
 * 1. Debounce input by checking three consecutive stable reads.
 * 2. Detect short press when state changes to PRESSED.
 * 3. Detect long press every 500ms while held.
 * ================================================================== */
void getKeyInput()
{
  // Ignore buttons for the first 100ms after startup
  if (startup_counter > 0)
  {
    startup_counter--;
    return;
  }

  for (int i = 0; i < 3; i++)
  {
    // Step 1: Shift previous readings
    KeyReg2[i] = KeyReg1[i];
    KeyReg1[i] = KeyReg0[i];

    // Step 2: Read new value from GPIO
    switch (i)
    {
    case 0:
      KeyReg0[i] = HAL_GPIO_ReadPin(button1_GPIO_Port, button1_Pin);
      break;
    case 1:
      KeyReg0[i] = HAL_GPIO_ReadPin(button2_GPIO_Port, button2_Pin);
      break;
    case 2:
      KeyReg0[i] = HAL_GPIO_ReadPin(button3_GPIO_Port, button3_Pin);
      break;
    }

    // Step 3: Debouncing – verify 3 consecutive stable readings
    if ((KeyReg1[i] == KeyReg0[i]) && (KeyReg1[i] == KeyReg2[i]))
    {
      // Step 4: Detect state change
      if (KeyReg3[i] != KeyReg2[i])
      {
        KeyReg3[i] = KeyReg2[i];

        if (KeyReg3[i] == PRESSED_STATE)
        {
          subKeyProcess(i);
          TimeOutForKeyPress[i] = 100;
        }
      }
      else
      {
        // Step 5: Handle long press
        TimeOutForKeyPress[i]--;

        if (TimeOutForKeyPress[i] == 0)
        {
          TimeOutForKeyPress[i] = 100;

          if (KeyReg3[i] == PRESSED_STATE)
          {
            button_long_pressed[i] = 1;
          }
        }
      }
    }
  }
}
