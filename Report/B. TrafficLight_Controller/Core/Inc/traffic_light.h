///*
// * traffic_light.h
// * Header file cho hệ thống đèn giao thông
// */
//
//#ifndef INC_TRAFFIC_LIGHT_H_
//#define INC_TRAFFIC_LIGHT_H_
//
////#include "main.h"
//
///* ==================================================================
// * ENUM DEFINITIONS
// * ================================================================== */
//
//// Chế độ hoạt động
//enum MODE {
//    MODE_1_NORMAL = 1,
//    MODE_2_RED_MODIFY,
//    MODE_3_AMBER_MODIFY,
//    MODE_4_GREEN_MODIFY
//};
//
//// Trạng thái đèn giao thông
//enum TRAFFIC_STATE {
//    INIT,
//    RED_GREEN,      // Đường 1: Đỏ, Đường 2: Xanh
//    RED_AMBER,      // Đường 1: Đỏ, Đường 2: Vàng
//    GREEN_RED,      // Đường 1: Xanh, Đường 2: Đỏ
//    AMBER_RED       // Đường 1: Vàng, Đường 2: Đỏ
//};
//
///* ==================================================================
// * BIẾN TOÀN CỤC (EXTERN)
// * ================================================================== */
//extern int duration_RED;
//extern int duration_AMBER;
//extern int duration_GREEN;
//extern int temp_duration;
//extern int counter_road1;
//extern int counter_road2;
//extern enum MODE current_mode;
//extern enum TRAFFIC_STATE traffic_state;
//
///* ==================================================================
// * FUNCTION PROTOTYPES
// * ================================================================== */
//
//// Hàm khởi tạo
//void traffic_init(void);
//
//// Hàm chính - gọi trong timer interrupt
//void traffic_run(void);
//
//// FSM cho các mode
//void fsm_normal_mode(void);
//void fsm_red_modify_mode(void);
//void fsm_amber_modify_mode(void);
//void fsm_green_modify_mode(void);
//
//// Xử lý button (edge detection)
//void update_button_state(void);
//
//// Xử lý LED blinking
//void handle_led_blinking(int led_type);
//
//// Cập nhật hiển thị
//void update_led_display(void);
//void update_7seg_display(void);
//
//// Điều khiển LED giao thông
//void turn_off_all_leds(void);
//void set_traffic_led(int road, int red, int amber, int green);
//void displayLED_RED(int IS_ON, int index);
//void displayLED_YELLOW(int IS_ON, int index);
//void displayLED_GREEN(int IS_ON, int index);
//
//// Điều khiển LED 7 đoạn
//void display_7seg_left(int num);
//void display_7seg_right(int num);
//void display_7seg_mode(int mode);
//
//#endif /* INC_TRAFFIC_LIGHT_H_ */
