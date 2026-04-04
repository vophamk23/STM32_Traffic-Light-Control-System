///*
// * traffic_light.c
// * Implementation hệ thống đèn giao thông - FIXED VERSION
// * Tuân theo cấu trúc code mẫu
// */
//
//#include "traffic_light.h"
//#include "software_timer.h"
//#include "button.h"
//
///* ==================================================================
// * BIẾN TOÀN CỤC
// * ================================================================== */
//int duration_RED = 5;       // 5 giây
//int duration_AMBER = 2;     // 2 giây
//int duration_GREEN = 3;     // 3 giây
//
//int temp_duration = 0;
//int counter_road1 = 0;
//int counter_road2 = 0;
//
//enum MODE current_mode = MODE_1_NORMAL;
//enum TRAFFIC_STATE traffic_state = INIT;
//
//// Biến cho LED blinking
//int blink_counter = 0;
//int MAX_BLINK_COUNTER = 50;  // 500ms / 10ms
//int flag_blink = 0;
//
//// Biến cho LED state (theo code mẫu)
//int flagRed[2] = {0, 0};
//int flagGreen[2] = {0, 0};
//int flagYellow[2] = {0, 0};
//
//// Button state tracking (edge detection như code mẫu)
//enum BUTTON_STATE {BTN_RELEASE, BTN_PRESS};
//enum BUTTON_STATE prevState[3] = {BTN_RELEASE, BTN_RELEASE, BTN_RELEASE};
//enum BUTTON_STATE currState[3] = {BTN_RELEASE, BTN_RELEASE, BTN_RELEASE};
//
///* ==================================================================
// * HÀM KHỞI TẠO
// * ================================================================== */
//void traffic_init(void)
//{
//    current_mode = MODE_1_NORMAL;
//    traffic_state = INIT;
//
//    // Tắt tất cả LED
//    turn_off_all_leds();
//
//    // Khởi tạo counter
//    counter_road1 = 0;
//    counter_road2 = 0;
//
//    // QUAN TRỌNG: Reset button state về RELEASE
//    prevState[0] = BTN_RELEASE;
//    prevState[1] = BTN_RELEASE;
//    prevState[2] = BTN_RELEASE;
//    currState[0] = BTN_RELEASE;
//    currState[1] = BTN_RELEASE;
//    currState[2] = BTN_RELEASE;
//
//    // Reset blink counter
//    blink_counter = 0;
//    flag_blink = 0;
//
//    // Reset LED flags
//    flagRed[0] = flagRed[1] = 0;
//    flagGreen[0] = flagGreen[1] = 0;
//    flagYellow[0] = flagYellow[1] = 0;
//}
//
///* ==================================================================
// * HÀM CHÍNH - GỌI TRONG TIMER INTERRUPT (10ms)
// * Theo code mẫu: logic chạy trong HAL_TIM_PeriodElapsedCallback
// * ================================================================== */
//void traffic_run(void)
//{
//    // Cập nhật button state (edge detection)
//    update_button_state();
//
//    // Xử lý theo mode
//    switch(current_mode) {
//        case MODE_1_NORMAL:
//            fsm_normal_mode();
//            break;
//
//        case MODE_2_RED_MODIFY:
//            fsm_red_modify_mode();
//            break;
//
//        case MODE_3_AMBER_MODIFY:
//            fsm_amber_modify_mode();
//            break;
//
//        case MODE_4_GREEN_MODIFY:
//            fsm_green_modify_mode();
//            break;
//    }
//
//    // Update LED display
//    update_led_display();
//
//    // Update 7-segment display
//    update_7seg_display();
//}
//
///* ==================================================================
// * EDGE DETECTION CHO BUTTON (theo code mẫu)
// * ================================================================== */
//void update_button_state(void)
//{
//    for(int i = 0; i < 3; i++) {
//        prevState[i] = currState[i];
//
//        // Đọc trạng thái button (giả sử đã debounce trong button.c)
//        // CHÚ Ý: isButtonXPressed() trả về 1 nếu có sự kiện press
//        switch(i) {
//            case 0:
//                if(isButton1Pressed()) {
//                    currState[i] = BTN_PRESS;
//                } else {
//                    currState[i] = BTN_RELEASE;
//                }
//                break;
//
//            case 1:
//                if(isButton2Pressed()) {
//                    currState[i] = BTN_PRESS;
//                } else {
//                    currState[i] = BTN_RELEASE;
//                }
//                break;
//
//            case 2:
//                if(isButton3Pressed()) {
//                    currState[i] = BTN_PRESS;
//                } else {
//                    currState[i] = BTN_RELEASE;
//                }
//                break;
//        }
//    }
//}
//
///* ==================================================================
// * FSM MODE 1: HOẠT ĐỘNG BÌNH THƯỜNG
// * Logic đếm ngược mỗi 100 cycles (1 giây nếu cycle = 10ms)
// * ================================================================== */
//void fsm_normal_mode(void)
//{
//    static int timer_counter = 0;
//    static int TIMER_CYCLE = 100;  // 100 * 10ms = 1000ms = 1s
//
//    // Xử lý nút MODE (Button 1) - Chuyển sang chế độ điều chỉnh
//    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
//        current_mode = MODE_2_RED_MODIFY;
//        temp_duration = duration_RED;
//        turn_off_all_leds();
//        return;
//    }
//
//    // Đếm timer
//    timer_counter++;
//    if(timer_counter < TIMER_CYCLE) return;
//
//    timer_counter = 0;  // Reset counter
//
//    // FSM đèn giao thông
//    switch(traffic_state) {
//        case INIT:
//            traffic_state = RED_GREEN;
//            counter_road1 = duration_RED;
//            counter_road2 = duration_GREEN;
//            break;
//
//        case RED_GREEN:
//            counter_road1--;
//            counter_road2--;
//
//            if(counter_road2 <= 0) {
//                traffic_state = RED_AMBER;
//                counter_road1 = duration_AMBER;
//                counter_road2 = duration_AMBER;
//            }
//            break;
//
//        case RED_AMBER:
//            counter_road1--;
//            counter_road2--;
//
//            if(counter_road1 <= 0) {
//                traffic_state = GREEN_RED;
//                counter_road1 = duration_GREEN;
//                counter_road2 = duration_RED;
//            }
//            break;
//
//        case GREEN_RED:
//            counter_road1--;
//            counter_road2--;
//
//            if(counter_road1 <= 0) {
//                traffic_state = AMBER_RED;
//                counter_road1 = duration_AMBER;
//                counter_road2 = duration_AMBER;
//            }
//            break;
//
//        case AMBER_RED:
//            counter_road1--;
//            counter_road2--;
//
//            if(counter_road2 <= 0) {
//                traffic_state = RED_GREEN;
//                counter_road1 = duration_RED;
//                counter_road2 = duration_GREEN;
//            }
//            break;
//    }
//
//    // Đảm bảo counter không âm
//    if(counter_road1 < 0) counter_road1 = 0;
//    if(counter_road2 < 0) counter_road2 = 0;
//}
//
///* ==================================================================
// * FSM MODE 2: ĐIỀU CHỈNH ĐÈN ĐỎ (theo code mẫu)
// * ================================================================== */
//void fsm_red_modify_mode(void)
//{
//    // Xử lý nút MODE (Button 1) - Chuyển mode
//    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
//        current_mode = MODE_3_AMBER_MODIFY;
//        temp_duration = duration_AMBER;
//        return;
//    }
//
//    // Xử lý nút MODIFY (Button 2) - Tăng giá trị
//    if(currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE) {
//        temp_duration++;
//        if(temp_duration > 99) temp_duration = 1;
//        if(temp_duration + duration_AMBER + duration_GREEN > 99) {
//            temp_duration = 99 - duration_AMBER - duration_GREEN;
//        }
//    }
//
//    // Xử lý nút SET (Button 3) - Lưu và quay về MODE 1
//    if(currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE) {
//        duration_RED = temp_duration;
//        current_mode = MODE_1_NORMAL;
//        traffic_state = INIT;
//        turn_off_all_leds();
//        return;
//    }
//
//    // Blink LED đỏ (2Hz = 500ms ON/OFF)
//    handle_led_blinking(2);  // 2 = RED
//}
//
///* ==================================================================
// * FSM MODE 3: ĐIỀU CHỈNH ĐÈN VÀNG
// * ================================================================== */
//void fsm_amber_modify_mode(void)
//{
//    // Xử lý nút MODE (Button 1)
//    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
//        current_mode = MODE_4_GREEN_MODIFY;
//        temp_duration = duration_GREEN;
//        return;
//    }
//
//    // Xử lý nút MODIFY (Button 2)
//    if(currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE) {
//        temp_duration++;
//        if(temp_duration > 99) temp_duration = 1;
//        if(temp_duration + duration_RED + duration_GREEN > 99) {
//            temp_duration = 99 - duration_RED - duration_GREEN;
//        }
//    }
//
//    // Xử lý nút SET (Button 3)
//    if(currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE) {
//        duration_AMBER = temp_duration;
//        current_mode = MODE_1_NORMAL;
//        traffic_state = INIT;
//        turn_off_all_leds();
//        return;
//    }
//
//    // Blink LED vàng
//    handle_led_blinking(1);  // 1 = YELLOW
//}
//
///* ==================================================================
// * FSM MODE 4: ĐIỀU CHỈNH ĐÈN XANH
// * ================================================================== */
//void fsm_green_modify_mode(void)
//{
//    // Xử lý nút MODE (Button 1) - Quay về MODE 1
//    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
//        current_mode = MODE_1_NORMAL;
//        traffic_state = INIT;
//        turn_off_all_leds();
//        return;
//    }
//
//    // Xử lý nút MODIFY (Button 2)
//    if(currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE) {
//        temp_duration++;
//        if(temp_duration > 99) temp_duration = 1;
//        if(temp_duration + duration_RED + duration_AMBER > 99) {
//            temp_duration = 99 - duration_RED - duration_AMBER;
//        }
//    }
//
//    // Xử lý nút SET (Button 3)
//    if(currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE) {
//        duration_GREEN = temp_duration;
//        current_mode = MODE_1_NORMAL;
//        traffic_state = INIT;
//        turn_off_all_leds();
//        return;
//    }
//
//    // Blink LED xanh
//    handle_led_blinking(0);  // 0 = GREEN
//}
//
//
///* ==================================================================
// * XỬ LÝ LED BLINKING (2Hz = 500ms ON/OFF)
// * Theo code mẫu: dùng counter trong interrupt
// * QUAN TRỌNG: Chỉ 1 màu nhấp nháy, 2 màu còn lại phải TẮT HẲNG
// * CHÚ Ý: LED active LOW nên flag=1 để TẮT, flag=0 để SÁNG (nhấp nháy)
// * ================================================================== */
//void handle_led_blinking(int led_type)
//{
//    blink_counter++;
//
//    if(blink_counter >= MAX_BLINK_COUNTER) {
//        blink_counter = 0;
//        flag_blink = !flag_blink;
//
//        // LUÔN LUÔN TẮT TẤT CẢ CÁC MÀU TRƯỚC (set = 1 vì active LOW)
//        flagRed[0] = 1;
//        flagRed[1] = 1;
//        flagGreen[0] = 1;
//        flagGreen[1] = 1;
//        flagYellow[0] = 1;
//        flagYellow[1] = 1;
//
//        // CHỈ BẬT màu đang được chỉnh khi flag_blink = 1
//        // flag_blink toggle giữa 0 (SÁNG) và 1 (TẮT) do active LOW
//        switch(led_type) {
//            case 0:  // MODE 4: CHỈ XANH nhấp nháy
//                flagGreen[0] = flag_blink;
//                flagGreen[1] = flag_blink;
//                // Đỏ và Vàng = 1 (TẮT) ở trên
//                break;
//
//            case 1:  // MODE 3: CHỈ VÀNG nhấp nháy
//                flagYellow[0] = flag_blink;
//                flagYellow[1] = flag_blink;
//                // Đỏ và Xanh = 1 (TẮT) ở trên
//                break;
//
//            case 2:  // MODE 2: CHỈ ĐỎ nhấp nháy
//                flagRed[0] = flag_blink;
//                flagRed[1] = flag_blink;
//                // Vàng và Xanh = 1 (TẮT) ở trên
//                break;
//        }
//    }
//}
//
///* ==================================================================
// * CẬP NHẬT LED DISPLAY
// * ================================================================== */
//void update_led_display(void)
//{
//    if(current_mode == MODE_1_NORMAL) {
//        // Hiển thị theo traffic state
//        switch(traffic_state) {
//            case INIT:
//                turn_off_all_leds();
//                break;
//
//            case RED_GREEN:
//                set_traffic_led(0, 1, 0, 0);  // Đường 1: Đỏ
//                set_traffic_led(1, 0, 0, 1);  // Đường 2: Xanh
//                break;
//
//            case RED_AMBER:
//                set_traffic_led(0, 1, 0, 0);  // Đường 1: Đỏ
//                set_traffic_led(1, 0, 1, 0);  // Đường 2: Vàng
//                break;
//
//            case GREEN_RED:
//                set_traffic_led(0, 0, 0, 1);  // Đường 1: Xanh
//                set_traffic_led(1, 1, 0, 0);  // Đường 2: Đỏ
//                break;
//
//            case AMBER_RED:
//                set_traffic_led(0, 0, 1, 0);  // Đường 1: Vàng
//                set_traffic_led(1, 1, 0, 0);  // Đường 2: Đỏ
//                break;
//        }
//    }
//    else {
//        // MODE điều chỉnh - hiển thị LED blink
//        displayLED_RED(flagRed[0], 0);
//        displayLED_RED(flagRed[1], 1);
//        displayLED_YELLOW(flagYellow[0], 0);
//        displayLED_YELLOW(flagYellow[1], 1);
//        displayLED_GREEN(flagGreen[0], 0);
//        displayLED_GREEN(flagGreen[1], 1);
//    }
//}
//
///* ==================================================================
// * ĐIỀU KHIỂN LED GIAO THÔNG (theo code mẫu)
// * ================================================================== */
//void turn_off_all_leds(void)
//{
//    HAL_GPIO_WritePin(GPIOA, RED1_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOA, RED2_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, GPIO_PIN_RESET);
//}
//
//
//void set_traffic_led(int road, int red, int amber, int green)
//{
//    if(road == 0) {  // Đường 1
//        HAL_GPIO_WritePin(GPIOA, RED1_Pin, red ? GPIO_PIN_RESET : GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, amber ? GPIO_PIN_RESET : GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, green ? GPIO_PIN_RESET : GPIO_PIN_SET);
//    } else {  // Đường 2
//        HAL_GPIO_WritePin(GPIOA, RED2_Pin, red ? GPIO_PIN_RESET : GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, amber ? GPIO_PIN_RESET : GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, green ? GPIO_PIN_RESET : GPIO_PIN_SET);
//    }
//}
//
//void displayLED_RED(int IS_ON, int index)
//{
//    switch(index) {
//        case 0:
//            HAL_GPIO_WritePin(GPIOA, RED1_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
//            break;
//        case 1:
//            HAL_GPIO_WritePin(GPIOA, RED2_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
//            break;
//    }
//}
//
//void displayLED_YELLOW(int IS_ON, int index)
//{
//    switch(index) {
//        case 0:
//            HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
//            break;
//        case 1:
//            HAL_GPIO_WritePin(GPIOA, YELLOW2_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
//            break;
//    }
//}
//
//void displayLED_GREEN(int IS_ON, int index)
//{
//    switch(index) {
//        case 0:
//            HAL_GPIO_WritePin(GPIOA, GREEN1_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
//            break;
//        case 1:
//            HAL_GPIO_WritePin(GPIOA, GREEN2_Pin, IS_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
//            break;
//    }
//}
//
///* ==================================================================
// * HIỂN THỊ LED 7 ĐOẠN (theo code mẫu - dùng BCD)
// * ================================================================== */
//void display_7seg_left(int num)
//{
//    int tens = num / 10;
//    int units = num % 10;
//
//    // SEG0 - Hàng chục (PA12-PA15)
//    HAL_GPIO_WritePin(GPIOA, inputseg0_0_Pin, (tens & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOA, inputseg0_1_Pin, (tens & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOA, inputseg0_2_Pin, (tens & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOA, inputseg0_3_Pin, (tens & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//
//    // SEG1 - Hàng đơn vị (PB0-PB3)
//    HAL_GPIO_WritePin(GPIOB, inputseg1_0_Pin, (units & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputseg1_1_Pin, (units & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputseg1_2_Pin, (units & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputseg1_3_Pin, (units & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//}
//
//void display_7seg_right(int num)
//{
//    int tens = num / 10;
//    int units = num % 10;
//
//    // SEG2 - Hàng chục (PB4-PB7)
//    HAL_GPIO_WritePin(GPIOB, inputseg2_0_Pin, (tens & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputseg2_1_Pin, (tens & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputseg2_2_Pin, (tens & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputseg2_3_Pin, (tens & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//
//    // SEG3 - Hàng đơn vị (PB8-PB11)
//    HAL_GPIO_WritePin(GPIOB, inputseg3_0_Pin, (units & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputseg3_1_Pin, (units & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputseg3_2_Pin, (units & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputseg3_3_Pin, (units & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//}
//
//void display_7seg_mode(int mode)
//{
//    // Hiển thị mode (PB12-PB15)
//    HAL_GPIO_WritePin(GPIOB, inputmode_0_Pin, (mode & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputmode_1_Pin, (mode & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputmode_2_Pin, (mode & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, inputmode_3_Pin, (mode & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//}
//
//void update_7seg_display(void)
//{
//    if(current_mode == MODE_1_NORMAL) {
//        // Hiển thị thời gian đếm ngược
//        display_7seg_left(counter_road1);
//        display_7seg_right(counter_road2);
//        display_7seg_mode(1);
//    } else {
//        // Hiển thị giá trị điều chỉnh và mode
//        display_7seg_left(temp_duration);
//        display_7seg_right(temp_duration);
//        display_7seg_mode(current_mode);
//    }
//}
