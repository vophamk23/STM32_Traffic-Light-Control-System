/*
 * scheduler.c - Cooperative Task Scheduler
 *
 * Cách hoạt động:
 *   - Timer interrupt (10ms) gọi SCH_Update() → O(1), chỉ giảm delay task[0]
 *   - Main loop gọi SCH_Dispatch_Tasks() → thực thi task khi đến giờ
 *
 * Mảng task luôn được sắp xếp theo Delay tăng dần.
 * MARKING[] đánh dấu các task có cùng delay với task[0] (chạy cùng lúc).
 */

#include "scheduler.h"

/* ==================== BIẾN TOÀN CỤC ==================== */

sTask SCH_tasks_G[SCH_MAX_TASKS];
uint8_t Error_code_G = 0;
uint8_t MARKING[SCH_MAX_TASKS];
uint32_t task_count = 0;
uint32_t elapsed_time = 0;

/* ==================== PRIVATE ==================== */
static void SCH_Update_Marking(void);

/* ============================================================
 * SCH_Init - Xóa tất cả task, reset biến
 * Gọi 1 lần trong main() trước khi Add_Task
 * ============================================================ */
void SCH_Init(void)
{
    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++)
    {
        SCH_tasks_G[i].pTask = 0x0000;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe = 0;
        SCH_tasks_G[i].TaskID = 0;
        MARKING[i] = 0;
    }
    task_count = 0;
    elapsed_time = 0;
    Error_code_G = 0;
}

/* ============================================================
 * SCH_Add_Task - Thêm task (mảng luôn sắp xếp theo Delay tăng dần)
 *   pFunction : hàm cần gọi
 *   DELAY     : số tick chờ trước lần chạy đầu
 *   PERIOD    : số tick giữa các lần chạy (0 = chỉ chạy 1 lần)
 * Trả về: index của task trong mảng
 * ============================================================ */
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD)
{
    if (task_count == 0)
    {
        SCH_tasks_G[0].pTask = pFunction;
        SCH_tasks_G[0].Delay = DELAY;
        SCH_tasks_G[0].Period = PERIOD;
        SCH_tasks_G[0].RunMe = 0;
        SCH_tasks_G[0].TaskID = 0;
        MARKING[0] = 1;
        elapsed_time = 0;
        task_count++;
        return 0;
    }

    if (task_count >= SCH_MAX_TASKS)
    {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        return SCH_MAX_TASKS;
    }

    // Tìm vị trí chèn để giữ mảng sắp xếp theo Delay
    uint32_t insert_index = 0;
    for (uint32_t i = 0; i < task_count; i++)
    {
        if (DELAY >= SCH_tasks_G[i].Delay)
            insert_index = i + 1;
        else
            break;
    }

    // Dịch các task sau vị trí chèn sang phải
    for (int32_t j = task_count; j > (int32_t)insert_index; j--)
    {
        SCH_tasks_G[j] = SCH_tasks_G[j - 1];
    }

    SCH_tasks_G[insert_index].pTask = pFunction;
    SCH_tasks_G[insert_index].Delay = DELAY;
    SCH_tasks_G[insert_index].Period = PERIOD;
    SCH_tasks_G[insert_index].RunMe = 0;
    SCH_tasks_G[insert_index].TaskID = insert_index;

    task_count++;
    SCH_Update_Marking();

    return insert_index;
}

/* ============================================================
 * SCH_Delete_Task - Xóa task theo index
 * ============================================================ */
uint8_t SCH_Delete_Task(const uint32_t TASK_INDEX)
{
    if (TASK_INDEX >= task_count || task_count == 0)
    {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
        return RETURN_ERROR;
    }

    if (task_count == 1)
    {
        SCH_tasks_G[0].pTask = 0x0000;
        SCH_tasks_G[0].Delay = 0;
        SCH_tasks_G[0].Period = 0;
        SCH_tasks_G[0].RunMe = 0;
        SCH_tasks_G[0].TaskID = 0;
        MARKING[0] = 0;
        task_count = 0;
    }
    else
    {
        // Dịch các task sau TASK_INDEX sang trái
        for (uint32_t k = TASK_INDEX; k < task_count - 1; k++)
        {
            SCH_tasks_G[k] = SCH_tasks_G[k + 1];
        }
        uint32_t last = task_count - 1;
        SCH_tasks_G[last].pTask = 0x0000;
        SCH_tasks_G[last].Delay = 0;
        SCH_tasks_G[last].Period = 0;
        SCH_tasks_G[last].RunMe = 0;
        SCH_tasks_G[last].TaskID = 0;
        MARKING[last] = 0;
        task_count--;

        if (task_count > 0)
            SCH_Update_Marking();
    }

    return RETURN_NORMAL;
}

/* ============================================================
 * SCH_Update_Marking (private)
 * Đánh dấu các task có cùng Delay với task[0] → sẽ chạy cùng lúc
 * ============================================================ */
static void SCH_Update_Marking(void)
{
    if (task_count == 0)
        return;
    uint32_t first_delay = SCH_tasks_G[0].Delay;
    for (uint32_t n = 0; n < task_count; n++)
    {
        MARKING[n] = (SCH_tasks_G[n].Delay == first_delay) ? 1 : 0;
    }
}

/* ============================================================
 * SCH_Update - GỌI TRONG TIMER INTERRUPT (mỗi 10ms)
 * O(1): Chỉ giảm Delay của task[0] (task gần nhất)
 * ============================================================ */
void SCH_Update(void)
{
    if (task_count > 0)
    {
        if (SCH_tasks_G[0].Delay > 0)
        {
            SCH_tasks_G[0].Delay--;
        }
        elapsed_time++;
        if (SCH_tasks_G[0].Delay == 0)
        {
            SCH_tasks_G[0].RunMe++;
        }
    }
}

/* ============================================================
 * SCH_Dispatch_Tasks - GỌI TRONG MAIN LOOP
 *
 * Bước 1: Cập nhật Delay tất cả task dựa trên elapsed_time
 *   - Task có MARKING=1 (cùng lúc với task[0]) → đặt RunMe=1
 *   - Task có MARKING=0 → trừ đi elapsed_time
 * Bước 2: Thực thi các task có RunMe > 0
 *   - One-shot (Period=0): xóa sau khi chạy
 *   - Periodic: xóa rồi thêm lại với Delay=Period
 * ============================================================ */
void SCH_Dispatch_Tasks(void)
{
    if (task_count > 0 && SCH_tasks_G[0].RunMe > 0)
    {

        // Bước 1: Cập nhật delay cho tất cả task
        for (uint32_t m = 0; m < task_count; m++)
        {
            if (MARKING[m] == 0)
            {
                // Trừ thời gian đã trôi qua, không cho về âm
                SCH_tasks_G[m].Delay = (SCH_tasks_G[m].Delay >= elapsed_time)
                                           ? SCH_tasks_G[m].Delay - elapsed_time
                                           : 0;
            }
            else
            {
                // Task cùng thời điểm → cũng sẵn sàng chạy
                SCH_tasks_G[m].Delay = 0;
                SCH_tasks_G[m].RunMe = 1;
            }
        }

        // Bước 2: Thực thi các task sẵn sàng
        while (task_count > 0 && SCH_tasks_G[0].RunMe > 0)
        {
            if (SCH_tasks_G[0].pTask != 0x0000)
            {
                (*SCH_tasks_G[0].pTask)();
            }
            SCH_tasks_G[0].RunMe--;

            if (SCH_tasks_G[0].Period == 0)
            {
                // One-shot: xóa luôn
                SCH_Delete_Task(0);
            }
            else
            {
                // Periodic: xóa rồi thêm lại
                void (*fn)(void) = SCH_tasks_G[0].pTask;
                uint32_t period = SCH_tasks_G[0].Period;
                SCH_Delete_Task(0);
                SCH_Add_Task(fn, period, period);
            }
        }
    }

    // (nếu không reset, khi task ready lần sau sẽ bị trừ delay sai)
    elapsed_time = 0;
}

/* ============================================================
 * Các hàm phụ trợ (optional)
 * ============================================================ */
void SCH_Report_Status(void)
{
    // TODO: Hiển thị Error_code_G qua LED hoặc UART
}

void SCH_Go_To_Sleep(void)
{
    // TODO: HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

uint32_t SCH_Get_Current_Size(void)
{
    return task_count;
}