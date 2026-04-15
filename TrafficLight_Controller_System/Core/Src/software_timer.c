/*
 * software_timer.c - Timer phần mềm (tối đa 10 timer)
 * setTimer() : nạp giá trị đếm
 * timerRun() : gọi mỗi tick để đếm ngược (trong Timer interrupt)
 * isTimerExpired() : kiểm tra hết hạn chưa (tự clear sau khi đọc)
 */

#include "software_timer.h"

#define MAX_COUNTER 10

static int timer_counter[MAX_COUNTER];
static int timer_flag[MAX_COUNTER];

/* Nạp timer index với value tick */
void setTimer(int index, int value)
{
    if (index < MAX_COUNTER) {
        timer_counter[index] = value;
        timer_flag[index]    = 0;
    }
}

/* Kiểm tra timer có hết hạn chưa (tự clear cờ) */
int isTimerExpired(int index)
{
    if (index < MAX_COUNTER && timer_flag[index]) {
        timer_flag[index] = 0;
        return 1;
    }
    return 0;
}

/* Đếm ngược tất cả timer - gọi trong Timer interrupt */
void timerRun(void)
{
    for (int i = 0; i < MAX_COUNTER; i++) {
        if (timer_counter[i] > 0) {
            if (--timer_counter[i] == 0) {
                timer_flag[i] = 1;
            }
        }
    }
}