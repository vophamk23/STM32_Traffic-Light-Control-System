/*
 * ============================================================================
 * COOPERATIVE SCHEDULER IMPLEMENTATION
 * ============================================================================
 * Mô tả: Scheduler với O(1) Update function để tối ưu cho interrupt
 *
 * ƯU ĐIỂM:
 * - SCH_Update() chạy trong O(1) → Nhanh trong interrupt
 * - Mảng task luôn được sắp xếp theo delay tăng dần
 * - Hỗ trợ cả periodic và one-shot tasks
 * ============================================================================
 */

#include "scheduler.h"

/* ==================== BIẾN TOÀN CỤC ==================== */

// Mảng chứa tất cả các task (đã sắp xếp theo Delay tăng dần)
sTask SCH_tasks_G[SCH_MAX_TASKS];

// Mã lỗi hệ thống (0 = không có lỗi)
uint8_t Error_code_G = 0;

// Mảng đánh dấu: MARKING[i]=1 nếu task[i] có cùng delay với task[0]
// VÍ DỤ: Nếu Task[0].Delay=10, Task[1].Delay=10, Task[2].Delay=20
//        → MARKING = [1, 1, 0]
uint8_t MARKING[SCH_MAX_TASKS];

// Số lượng task hiện đang hoạt động
uint32_t task_count = 0;

// Đếm số tick đã trôi qua kể từ lần dispatch cuối
// VÍ DỤ: Nếu elapsed_time=50 → đã qua 50 tick (500ms với tick=10ms)
uint32_t elapsed_time = 0;

/* ==================== HÀM PRIVATE (INTERNAL) ==================== */
static void SCH_Update_Marking(void);

/* ==================== IMPLEMENTATION ==================== */

/**
 * ============================================================================
 * HÀM: SCH_Init
 * ============================================================================
 * MÔ TẢ: Khởi tạo scheduler - Xóa tất cả task và reset biến
 *
 * GỌI KHI NÀO: Trong main(), trước khi thêm bất kỳ task nào
 *
 * VÍ DỤ:
 *   int main(void) {
 *       HAL_Init();
 *       SCH_Init();  // ← Gọi ở đây
 *       SCH_Add_Task(Task1, 0, 100);
 *       ...
 *   }
 * ============================================================================
 */
void SCH_Init(void) {
    // Xóa sạch tất cả các task trong mảng
    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask = 0x0000;    // Không có hàm nào
        SCH_tasks_G[i].Delay = 0;         // Delay = 0
        SCH_tasks_G[i].Period = 0;        // Period = 0
        SCH_tasks_G[i].RunMe = 0;         // Chưa cần chạy
        SCH_tasks_G[i].TaskID = 0;        // ID = 0
        MARKING[i] = 0;                   // Không đánh dấu
    }

    // Reset các biến đếm
    task_count = 0;        // Chưa có task nào
    elapsed_time = 0;      // Chưa đếm thời gian
    Error_code_G = 0;      // Không có lỗi
}

/**
 * ============================================================================
 * HÀM: SCH_Add_Task
 * ============================================================================
 * MÔ TẢ: Thêm task vào scheduler (mảng luôn được sắp xếp theo Delay)
 *
 * THAM SỐ:
 *   - pFunction: Con trỏ tới hàm cần gọi (kiểu void function(void))
 *   - DELAY: Số tick chờ trước khi chạy lần đầu
 *   - PERIOD: Số tick giữa các lần chạy (0 = chỉ chạy 1 lần)
 *
 * TRẢ VỀ: Index của task trong mảng (dùng để delete sau này)
 *
 * ĐỘ PHỨC TẠP: O(n) do phải tìm vị trí chèn và dịch mảng
 *
 * VÍ DỤ 1 - Task chạy ngay và lặp lại:
 *   SCH_Add_Task(LED_Toggle, 0, 100);
 *   → LED_Toggle chạy ngay, sau đó lặp mỗi 100 tick (1 giây nếu tick=10ms)
 *
 * VÍ DỤ 2 - Task chờ rồi lặp:
 *   SCH_Add_Task(Read_Sensor, 50, 200);
 *   → Chờ 50 tick (500ms), sau đó chạy và lặp mỗi 200 tick (2 giây)
 *
 * VÍ DỤ 3 - Task chạy 1 lần (one-shot):
 *   SCH_Add_Task(Send_Alert, 300, 0);
 *   → Chờ 300 tick (3 giây), chạy 1 lần rồi tự động xóa
 *
 * VÍ DỤ 4 - Lưu ID để xóa sau:
 *   uint32_t id = SCH_Add_Task(Buzzer, 0, 50);
 *   // Sau đó có thể xóa:
 *   SCH_Delete_Task(id);
 * ============================================================================
 */
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {

    /* ========== CASE 1: TASK ĐẦU TIÊN (MẢNG RỖNG) ========== */
    if (task_count == 0) {
        // Thêm task vào vị trí đầu tiên
        SCH_tasks_G[0].pTask = pFunction;
        SCH_tasks_G[0].Delay = DELAY;
        SCH_tasks_G[0].Period = PERIOD;
        SCH_tasks_G[0].RunMe = 0;
        SCH_tasks_G[0].TaskID = 0;
        MARKING[0] = 1;                    // Đánh dấu là task đầu
        elapsed_time = 0;
        task_count++;
        return 0;
    }

    /* ========== CASE 2: MẢNG ĐÃ ĐẦY ========== */
    if (task_count >= SCH_MAX_TASKS) {
        // Không thể thêm task nữa
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        return SCH_MAX_TASKS;
    }

    /* ========== CASE 3: TÌM VỊ TRÍ CHÈN (INSERTION SORT) ========== */
    /*
     * VÍ DỤ: Mảng hiện tại [Delay: 10, 20, 50, 100]
     *        Thêm task mới có Delay=30
     *
     * BƯỚC 1: Tìm vị trí chèn
     *   - 30 >= 10 ✅ → insert_index = 1
     *   - 30 >= 20 ✅ → insert_index = 2
     *   - 30 >= 50 ❌ → DỪNG
     *   → insert_index = 2 (chèn giữa 20 và 50)
     *
     * BƯỚC 2: Dịch mảng
     *   [10, 20, 50, 100, ?]
     *   [10, 20, _, 50, 100]  ← Dịch 50 và 100 sang phải
     *
     * BƯỚC 3: Chèn
     *   [10, 20, 30, 50, 100]  ← Mảng vẫn được sắp xếp!
     */

    uint32_t insert_index = 0;

    // Tìm vị trí để DELAY được sắp xếp tăng dần
    for (uint32_t i = 0; i < task_count; i++) {
        if (DELAY >= SCH_tasks_G[i].Delay) {
            insert_index = i + 1;  // Chèn sau task này
        } else {
            break;  // Dừng khi gặp task có delay lớn hơn
        }
    }

    // Dịch chuyển các task từ vị trí chèn sang phải 1 ô
    for (int32_t j = task_count; j > insert_index; j--) {
        SCH_tasks_G[j] = SCH_tasks_G[j - 1];
    }

    // Chèn task mới vào vị trí đã tìm được
    SCH_tasks_G[insert_index].pTask = pFunction;
    SCH_tasks_G[insert_index].Delay = DELAY;
    SCH_tasks_G[insert_index].Period = PERIOD;
    SCH_tasks_G[insert_index].RunMe = 0;
    SCH_tasks_G[insert_index].TaskID = insert_index;

    task_count++;

    // Cập nhật mảng MARKING (đánh dấu task nào có cùng delay với task[0])
    SCH_Update_Marking();

    return insert_index;
}

/**
 * ============================================================================
 * HÀM: SCH_Delete_Task
 * ============================================================================
 * MÔ TẢ: Xóa task khỏi scheduler
 *
 * THAM SỐ:
 *   - TASK_INDEX: Vị trí của task trong mảng (ID trả về từ Add_Task)
 *
 * TRẢ VỀ:
 *   - RETURN_NORMAL: Xóa thành công
 *   - RETURN_ERROR: Lỗi (index không hợp lệ)
 *
 * ĐỘ PHỨC TẠP: O(n) do phải dịch mảng sang trái
 *
 * VÍ DỤ:
 *   uint32_t buzzer_id = SCH_Add_Task(Buzzer, 0, 100);
 *   // ... sau một thời gian
 *   SCH_Delete_Task(buzzer_id);  // Tắt buzzer
 * ============================================================================
 */
uint8_t SCH_Delete_Task(const uint32_t TASK_INDEX) {

    // Kiểm tra index có hợp lệ không
    if (TASK_INDEX >= task_count || task_count == 0) {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
        return RETURN_ERROR;
    }

    /* ========== CASE 1: CHỈ CÒN 1 TASK ========== */
    if (task_count == 1) {
        // Xóa sạch task duy nhất
        SCH_tasks_G[0].pTask = 0x0000;
        SCH_tasks_G[0].Delay = 0;
        SCH_tasks_G[0].Period = 0;
        SCH_tasks_G[0].RunMe = 0;
        SCH_tasks_G[0].TaskID = 0;
        MARKING[0] = 0;
        task_count = 0;
    }
    /* ========== CASE 2: NHIỀU TASK → DỊCH TRÁI ========== */
    else {
        /*
         * VÍ DỤ: Xóa task ở vị trí 2
         * Trước: [A, B, C, D, E]
         *              ↑ Xóa C
         * Sau:  [A, B, D, E, _]
         *              ↑ D dịch sang trái
         */

        // Dịch tất cả các task sau task bị xóa sang trái 1 vị trí
        for (uint32_t k = TASK_INDEX; k < task_count - 1; k++) {
            SCH_tasks_G[k] = SCH_tasks_G[k + 1];
        }

        // Xóa sạch vị trí cuối cùng (đã không dùng nữa)
        uint32_t last_pos = task_count - 1;
        SCH_tasks_G[last_pos].pTask = 0x0000;
        SCH_tasks_G[last_pos].Delay = 0;
        SCH_tasks_G[last_pos].Period = 0;
        SCH_tasks_G[last_pos].RunMe = 0;
        SCH_tasks_G[last_pos].TaskID = 0;
        MARKING[last_pos] = 0;

        task_count--;

        // Cập nhật lại mảng MARKING
        if (task_count > 0) {
            SCH_Update_Marking();
        }
    }

    return RETURN_NORMAL;
}

/**
 * ============================================================================
 * HÀM: SCH_Update_Marking (PRIVATE)
 * ============================================================================
 * MÔ TẢ: Cập nhật mảng MARKING - đánh dấu task nào có cùng delay với task[0]
 *
 * TẠI SAO CẦN MARKING?
 *   - Để biết những task nào sẽ "sẵn sàng chạy cùng lúc"
 *   - Giúp cập nhật hàng loạt trong Dispatch
 *
 * VÍ DỤ:
 *   Task[0]: Delay=10  → MARKING[0]=1 ✅
 *   Task[1]: Delay=10  → MARKING[1]=1 ✅ (cùng delay với task[0])
 *   Task[2]: Delay=20  → MARKING[2]=0 ❌
 *   Task[3]: Delay=30  → MARKING[3]=0 ❌
 *
 *   Kết quả: MARKING = [1, 1, 0, 0]
 *   → 2 task đầu sẽ sẵn sàng cùng lúc!
 * ============================================================================
 */
static void SCH_Update_Marking(void) {
    if (task_count == 0) return;

    // Lấy delay của task đầu tiên làm chuẩn
    uint32_t first_delay = SCH_tasks_G[0].Delay;

    // Duyệt tất cả task và đánh dấu
    for (uint32_t n = 0; n < task_count; n++) {
        if (SCH_tasks_G[n].Delay == first_delay) {
            MARKING[n] = 1;  // ✅ Đánh dấu: cùng delay với task đầu
        } else {
            MARKING[n] = 0;  // ❌ Không đánh dấu: delay khác
        }
    }
}

/**
 * ============================================================================
 * HÀM: SCH_Update
 * ============================================================================
 * MÔ TẢ: Hàm cập nhật - GỌI TRONG INTERRUPT TIMER
 *
 * ĐỘ PHỨC TẠP: O(1) ⚡ - KEY OPTIMIZATION!
 *
 * CÁCH HOẠT ĐỘNG:
 *   1. CHỈ giảm delay của task đầu tiên (task sớm nhất)
 *   2. Tăng bộ đếm thời gian đã trôi qua
 *   3. Nếu task đầu đã đến giờ → đặt cờ RunMe
 *
 * TẠI SAO CHỈ CẬP NHẬT TASK ĐẦU?
 *   - Mảng đã được sắp xếp theo delay
 *   - Task[0] LUÔN là task sớm nhất
 *   - Các task khác sẽ được cập nhật trong Dispatch
 *   → Tiết kiệm thời gian trong interrupt!
 *
 * GỌI TỪ ĐÂU:
 *   void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
 *       if (htim->Instance == TIM2) {
 *           SCH_Update();  // ← Gọi ở đây
 *       }
 *   }
 *
 * VÍ DỤ TIMELINE (Tick = 10ms):
 *   Ban đầu:
 *     Task[0]: Delay=3
 *     Task[1]: Delay=5
 *     elapsed_time = 0
 *
 *   Tick 1 (10ms):
 *     Task[0]: Delay=2  ← Giảm
 *     elapsed_time = 1
 *
 *   Tick 2 (20ms):
 *     Task[0]: Delay=1
 *     elapsed_time = 2
 *
 *   Tick 3 (30ms):
 *     Task[0]: Delay=0, RunMe=1  ← ĐẾN GIỜ! Đặt cờ
 *     elapsed_time = 3
 * ============================================================================
 */
void SCH_Update(void) {
    if (task_count > 0) {
        // CHỈ giảm delay của task đầu tiên
        if (SCH_tasks_G[0].Delay > 0) {
            SCH_tasks_G[0].Delay--;
        }

        // Đếm thời gian đã trôi qua (dùng trong Dispatch)
        elapsed_time++;

        // Nếu task đầu tiên đã đến giờ → đặt cờ RunMe
        if (SCH_tasks_G[0].Delay == 0) {
            SCH_tasks_G[0].RunMe++;
        }
    }
}

/**
 * ============================================================================
 * HÀM: SCH_Dispatch_Tasks
 * ============================================================================
 * MÔ TẢ: Thực thi các task đã sẵn sàng - GỌI TRONG MAIN LOOP
 *
 * ĐỘ PHỨC TẠP: O(n²) trong trường hợp xấu nhất
 *
 * CÁCH HOẠT ĐỘNG (2 BƯỚC):
 *
 *   BƯỚC 1: CẬP NHẬT DELAY CHO TẤT CẢ TASK
 *     - Những task có MARKING=1: Đặt Delay=0, RunMe=1
 *     - Những task có MARKING=0: Trừ đi thời gian đã trôi qua
 *
 *   BƯỚC 2: THỰC THI CÁC TASK SẴN SÀNG
 *     - Gọi hàm task
 *     - Nếu là one-shot (Period=0): Xóa task
 *     - Nếu là periodic: Xóa rồi thêm lại với delay mới
 *
 * GỌI TỪ ĐÂU:
 *   int main(void) {
 *       SCH_Init();
 *       SCH_Add_Task(...);
 *
 *       while(1) {
 *           SCH_Dispatch_Tasks();  // ← Gọi ở đây
 *       }
 *   }
 *
 * VÍ DỤ CHI TIẾT:
 *   Ban đầu (sau khi Update đặt cờ):
 *     Task[0]: {LED_Toggle, Delay=0, Period=100, RunMe=1} MARKING=1
 *     Task[1]: {Buzzer, Delay=0, Period=50, RunMe=0}      MARKING=1
 *     Task[2]: {Sensor, Delay=50, Period=200, RunMe=0}    MARKING=0
 *     elapsed_time = 100  ← Đã trôi qua 100 tick
 *
 *   BƯỚC 1: Sau khi Dispatch_Tasks - Cập nhật delay cho từng task
 *     Task[0]: Delay=0, RunMe=1      (MARKING=1 → giữ nguyên)
 *     Task[1]: Delay=0, RunMe=1      (MARKING=1 → đặt cờ!)
 *     Task[2]: Delay=50-100=-50 → 0  (MARKING=0 → trừ đi 100)
 *
 *   BƯỚC 2: Thực thi
 *     - Chạy LED_Toggle() → Xóa → Thêm lại với Delay=100
 *     - Chạy Buzzer() → Xóa → Thêm lại với Delay=50
 *     - Chạy Sensor() (nếu RunMe=1)
 *
 *   Kết quả:
 *     Task[0]: {Buzzer, Delay=50, Period=50}
 *     Task[1]: {LED_Toggle, Delay=100, Period=100}
 *     Task[2]: {Sensor, Delay=200, Period=200}
 *     elapsed_time = 0  ← Reset
 * ============================================================================
 */
void SCH_Dispatch_Tasks(void) {

    // Kiểm tra có task nào sẵn sàng không
    if (task_count > 0 && SCH_tasks_G[0].RunMe > 0) {

        /* ========== BƯỚC 1: CẬP NHẬT DELAY CHO TẤT CẢ TASK ========== */
        for (uint32_t m = 0; m < task_count; m++) {
            if (MARKING[m] == 0) {
                // Task có delay KHÁC với task đầu
                // → Trừ đi thời gian đã trôi qua
                if (SCH_tasks_G[m].Delay >= elapsed_time) {
                    SCH_tasks_G[m].Delay -= elapsed_time;
                } else {
                    SCH_tasks_G[m].Delay = 0;
                }
            } else {
                // Task có CÙNG delay với task đầu
                // → Cũng sẵn sàng chạy!
                SCH_tasks_G[m].Delay = 0;
                SCH_tasks_G[m].RunMe = 1;
            }
        }

        /* ========== BƯỚC 2: VÒNG LẶP THỰC THI CÁC TASK SẴN SÀNG ========== */
        while (task_count > 0 && SCH_tasks_G[0].RunMe > 0) {

            // 1. CHẠY TASK
            if (SCH_tasks_G[0].pTask != 0x0000) {
                (*SCH_tasks_G[0].pTask)();  // Gọi hàm task!
            }

            // 2. HẠ CỜ
            SCH_tasks_G[0].RunMe--;

            // 3. XỬ LÝ TASK DỰA VÀO PERIOD
            if (SCH_tasks_G[0].Period == 0) {
                /* ONE-SHOT TASK: Chỉ chạy 1 lần → XÓA */
                SCH_Delete_Task(0);
            } else {
                /* PERIODIC TASK: Lặp lại → XÓA rồi THÊM LẠI */

                // Lưu thông tin task
                void (*temp_func)(void) = SCH_tasks_G[0].pTask;
                uint32_t temp_period = SCH_tasks_G[0].Period;

                // Xóa task cũ
                SCH_Delete_Task(0);

                // Thêm lại task với delay = period
                // VÍ DỤ: Period=100 → Task sẽ chạy lại sau 100 tick
                SCH_Add_Task(temp_func, temp_period, temp_period);
            }
        }

        // Reset bộ đếm thời gian
        elapsed_time = 0;
    }

    // (Optional) Báo cáo lỗi và vào chế độ ngủ
    // SCH_Report_Status();
    // SCH_Go_To_Sleep();
}

/**
 * ============================================================================
 * HÀM: SCH_Report_Status
 * ============================================================================
 * MÔ TẢ: Báo cáo trạng thái hệ thống (tùy chọn)
 *
 * CÁCH SỬ DỤNG:
 *   - Hiển thị mã lỗi trên LED
 *   - Gửi qua UART để debug
 *   - Lưu vào log
 * ============================================================================
 */
void SCH_Report_Status(void) {
    // TODO: Implement error reporting
    // VÍ DỤ: Hiển thị Error_code_G trên 8 LED
    if (Error_code_G != 0) {
        // Xử lý lỗi
        // HAL_GPIO_WritePort(ERROR_PORT, Error_code_G);
    }
}

/**
 * ============================================================================
 * HÀM: SCH_Go_To_Sleep
 * ============================================================================
 * MÔ TẢ: Đưa MCU vào chế độ tiết kiệm năng lượng (tùy chọn)
 *
 * CÁCH HOẠT ĐỘNG:
 *   - MCU vào chế độ SLEEP
 *   - Timer interrupt vẫn chạy → đánh thức MCU
 *   - Tiết kiệm ~50% năng lượng
 *
 * VÍ DỤ:
 *   HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
 * ============================================================================
 */
void SCH_Go_To_Sleep(void) {
    // TODO: Implement sleep mode
    // HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

/**
 * ============================================================================
 * HÀM: SCH_Get_Current_Size
 * ============================================================================
 * MÔ TẢ: Lấy số lượng task hiện tại
 *
 * VÍ DỤ:
 *   uint32_t num_tasks = SCH_Get_Current_Size();
 *   printf("Có %d tasks đang chạy\n", num_tasks);
 * ============================================================================
 */
uint32_t SCH_Get_Current_Size(void) {
    return task_count;
}
