/*
 * ============================================================================
 * tasks.h - TASK FUNCTIONS HEADER
 * ============================================================================
 * Mô tả: Header file cho module tasks.c
 *        Khai báo tất cả các task function và dependencies
 * ============================================================================ */

#ifndef INC_TASKS_H_
#define INC_TASKS_H_

/* ==================== INCLUDES ==================== */
#include "main.h"
#include "button.h"           // Để dùng getKeyInput()
#include "fsm_traffic.h"      // Để dùng traffic_run()
#include "led_display.h"      // Để dùng update_led_display()
#include "7segment_display.h" // Để dùng update_7seg_display()
#include "global.h"           // Để truy cập biến toàn cục
#include "scheduler.h"        // Để dùng SCH_Get_Current_Size()

#include <stdio.h>            // Để dùng sprintf()
#include <string.h>           // Để dùng strlen()


/* ==================== TASK PERIOD DEFINITIONS ==================== */

/**
 * Định nghĩa chu kỳ task (tính bằng số tick)
 * 1 tick = 10ms (do timer interrupt 10ms)
 *
 * Công thức: PERIOD_VALUE = (Thời gian mong muốn ms) / 10
 *
 */

/* ==================== TASK TIMING CONSTANTS (in milliseconds) ==================== */
#define TASK_BUTTON_PERIOD_MS       10      // Quét nút nhấn mỗi 10ms
#define TASK_FSM_PERIOD_MS          10      // FSM đèn giao thông mỗi 10ms
#define TASK_DISPLAY_PERIOD_MS      50      // Cập nhật hiển thị mỗi 50ms

/* ==================== TASK TIMING IN TICKS ==================== */
// Assuming TIMER_TICK_MS = 10ms
#define TASK_BUTTON_PERIOD          (TASK_BUTTON_PERIOD_MS / TIMER_TICK_MS)   // = 1 tick
#define TASK_FSM_PERIOD             (TASK_FSM_PERIOD_MS / TIMER_TICK_MS)      // = 1 tick
#define TASK_DISPLAY_PERIOD         (TASK_DISPLAY_PERIOD_MS / TIMER_TICK_MS)  // = 5 ticks


/* ==================== CORE TASK FUNCTIONS (REQUIRED) ==================== */

/**
 * @brief Task quét nút nhấn - BẮT BUỘC
 * @note  Chạy mỗi 10ms để phát hiện nhấn chính xác
 * @usage SCH_Add_Task(Task_Button_Scan, 0, TASK_BUTTON_PERIOD);
 */
void Task_Button_Scan(void);

/**
 * @brief Task điều khiển FSM đèn giao thông - BẮT BUỘC
 * @note  Chạy mỗi 10ms để cập nhật trạng thái
 * @usage SCH_Add_Task(Task_Traffic_FSM, 0, TASK_FSM_PERIOD);
 */
void Task_Traffic_FSM(void);

/**
 * @brief Task cập nhật hiển thị LED và 7-segment - BẮT BUỘC
 * @note  Có thể chạy chậm hơn (50ms) để tiết kiệm CPU
 * @usage SCH_Add_Task(Task_Update_Display, 0, TASK_DISPLAY_PERIOD);
 */
void Task_Update_Display(void);




#endif /* INC_TASKS_H_ */
