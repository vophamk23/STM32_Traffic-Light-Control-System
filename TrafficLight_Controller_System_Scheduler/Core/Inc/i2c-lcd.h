/*
 * i2c-lcd.h
 * Software I2C LCD driver - PA0 (SCL), PA1 (SDA)
 */

#ifndef INC_I2C_LCD_H_
#define INC_I2C_LCD_H_

#include "main.h"
#include "global.h"

void lcd_init(void);
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);
void lcd_send_string(char *str);
void lcd_clear_display(void);
void lcd_goto_XY(int row, int col);
void lcd_update_display(void);

#endif /* INC_I2C_LCD_H_ */
