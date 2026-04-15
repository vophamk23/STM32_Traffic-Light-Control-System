/*
 * Tasks.c - Định nghĩa các task cho Scheduler
 */

#include "tasks.h"

/* Task 1: Quét nút nhấn - chạy mỗi 10ms (PERIOD=1) */
void Task_Button_Scan(void) { getKeyInput(); }

/* Task 2: Chạy FSM đèn giao thông - chạy mỗi 10ms (PERIOD=1) */
void Task_Traffic_FSM(void) { traffic_run(); }

/* Task 3: Cập nhật LED + 7-seg - chạy mỗi 50ms (PERIOD=5)
 * Có thể điều chỉnh TASK_DISPLAY_PERIOD trong tasks.h: */
void Task_Update_Display(void)
{
    update_led_display();
    update_7seg_display();
}