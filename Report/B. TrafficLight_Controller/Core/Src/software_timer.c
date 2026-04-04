/*
 * software_timer.c
 *
 *  Created on: Oct 10, 2025
 *      Author: ASUS
 */

#include "software_timer.h"

#define MAX_COUNTER 10 // Maximum number of software timers (supports up to 10 timers)

static int timer_counter[MAX_COUNTER]; // Stores remaining countdown values for each timer
static int timer_flag[MAX_COUNTER];    // Flags set when a timer expires

// --------------------------------------------------------
// setTimer()
// --------------------------------------------------------
/**
 * @brief  Start or reset a software timer.
 * @param  index: Timer index (0 to MAX_COUNTER - 1)
 * @param  value: Initial countdown value (in timer ticks, e.g. ms or 10ms)
 * @note   The unit depends on how often timerRun() is called.
 */
void setTimer(int index, int value)
{
    if (index < MAX_COUNTER)
    {
        timer_counter[index] = value; // Load countdown value
        timer_flag[index] = 0;        // Clear expiration flag
    }
}

// --------------------------------------------------------
// isTimerExpired()
// --------------------------------------------------------
/**
 * @brief  Check whether a timer has expired.
 * @param  index: Timer index (0 to MAX_COUNTER - 1)
 * @retval 1 if the timer has expired, 0 otherwise.
 * @note   The flag is automatically cleared after checking.
 */
int isTimerExpired(int index)
{
    if (index < MAX_COUNTER)
    {
        if (timer_flag[index] == 1)
        {
            timer_flag[index] = 0; // Clear flag to prevent repeated triggering
            return 1;              // Timer expired
        }
    }
    return 0; // Not expired or invalid index
}

// --------------------------------------------------------
// timerRun()
// --------------------------------------------------------
/**
 * @brief  Update all software timers.
 * @note   This function must be called periodically by a hardware timer interrupt.
 *         For example, if called every 1ms, each countdown unit equals 1ms.
 */
void timerRun()
{
    for (int i = 0; i < MAX_COUNTER; i++)
    {
        if (timer_counter[i] > 0)
        {
            timer_counter[i]--; // Decrement countdown
            if (timer_counter[i] <= 0)
            {
                timer_flag[i] = 1; // Set expiration flag when timer reaches zero
            }
        }
    }
}
