/*
 * scheduler.h
 * Cooperative Scheduler with O(1) Update Function
 */
#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>
#include "main.h"

/* ==================== CONFIGURATION ==================== */
#define SCH_MAX_TASKS           40
#define NO_TASK_ID              0
#define TIMER_TICK_MS           10      // 10ms timer tick

/* ==================== ERROR CODES ==================== */
#define ERROR_SCH_TOO_MANY_TASKS                    1
#define ERROR_SCH_CANNOT_DELETE_TASK                2
#define ERROR_SCH_WAITING_FOR_SLAVE_TO_ACK          3
#define ERROR_SCH_WAITING_FOR_START_COMMAND         4
#define ERROR_SCH_ONE_OR_MORE_SLAVES_DID_NOT_START  5
#define ERROR_SCH_LOST_SLAVE                        6
#define ERROR_SCH_CAN_BUS_ERROR                     7

/* ==================== RETURN CODES ==================== */
#define RETURN_ERROR            0
#define RETURN_NORMAL           1

/* ==================== TASK STRUCTURE ==================== */
typedef struct {
    void (*pTask)(void);        // Pointer to the task function
    uint32_t Delay;             // Delay (ticks) until function will run
    uint32_t Period;            // Interval (ticks) between runs
    uint8_t RunMe;              // Flag: incremented when task is due
    uint32_t TaskID;            // Task identifier
} sTask;

/* ==================== GLOBAL VARIABLES ==================== */
extern sTask SCH_tasks_G[SCH_MAX_TASKS];
extern uint8_t Error_code_G;
extern uint8_t MARKING[SCH_MAX_TASKS];
extern uint32_t task_count;
extern uint32_t elapsed_time;

/* ==================== FUNCTION PROTOTYPES ==================== */

/**
 * @brief Initialize the scheduler
 * Sets all tasks to initial state
 */
void SCH_Init(void);

/**
 * @brief Update function - called from timer ISR every TIMER_TICK_MS
 * Time Complexity: O(1) - only updates first task!
 * This is the key optimization for fast ISR execution
 */
void SCH_Update(void);

/**
 * @brief Dispatch tasks that are ready to run
 * Should be called in the main loop
 * Time Complexity: O(n²) worst case (trade-off for O(1) Update)
 */
void SCH_Dispatch_Tasks(void);

/**
 * @brief Add a task to the scheduler
 * @param pFunction: Pointer to task function
 * @param DELAY: Initial delay in ticks (TIMER_TICK_MS units)
 * @param PERIOD: Period for repetitive tasks (0 for one-shot)
 * @return Task index in array, or SCH_MAX_TASKS if failed
 *
 * Example: SCH_Add_Task(Task_LED1, 0, 50); // Run every 500ms
 */
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD);

/**
 * @brief Delete a task from the scheduler
 * @param TASK_INDEX: Index of task to delete
 * @return RETURN_NORMAL or RETURN_ERROR
 */
uint8_t SCH_Delete_Task(const uint32_t TASK_INDEX);

/**
 * @brief Report system status (optional)
 */
void SCH_Report_Status(void);

/**
 * @brief Put system to sleep (optional - for power saving)
 */
void SCH_Go_To_Sleep(void);

/* ==================== HELPER FUNCTIONS ==================== */

/**
 * @brief Get current number of tasks
 */
uint32_t SCH_Get_Current_Size(void);

#endif /* INC_SCHEDULER_H_ */
