/*
 * i2c-lcd.c
 * Software I2C (bit-bang) trên PA0 (SCL) và PA1 (SDA)
 */

#include "main.h"
#include "i2c-lcd.h"
#include "global.h"
#include <stdio.h>

#define SLAVE_ADDRESS_LCD   0x27
#define LCD_SCL_PORT        GPIOA
#define LCD_SCL_PIN         GPIO_PIN_0
#define LCD_SDA_PORT        GPIOA
#define LCD_SDA_PIN         GPIO_PIN_1

/* ============================================================================
 * SOFTWARE I2C - BIT BANG
 * ============================================================================ */

static void i2c_delay(void)
{
    for (volatile int i = 0; i < 10; i++); // ~5µs @ 8MHz HSI
}

static void SCL_HIGH(void) { HAL_GPIO_WritePin(LCD_SCL_PORT, LCD_SCL_PIN, GPIO_PIN_SET);   }
static void SCL_LOW(void)  { HAL_GPIO_WritePin(LCD_SCL_PORT, LCD_SCL_PIN, GPIO_PIN_RESET); }
static void SDA_HIGH(void) { HAL_GPIO_WritePin(LCD_SDA_PORT, LCD_SDA_PIN, GPIO_PIN_SET);   }
static void SDA_LOW(void)  { HAL_GPIO_WritePin(LCD_SDA_PORT, LCD_SDA_PIN, GPIO_PIN_RESET); }

static void i2c_start(void)
{
    SDA_HIGH(); i2c_delay();
    SCL_HIGH(); i2c_delay();
    SDA_LOW();  i2c_delay(); // SDA xuống khi SCL cao = START
    SCL_LOW();  i2c_delay();
}

static void i2c_stop(void)
{
    SDA_LOW();  i2c_delay();
    SCL_HIGH(); i2c_delay();
    SDA_HIGH(); i2c_delay(); // SDA lên khi SCL cao = STOP
}

static void i2c_write_byte(uint8_t byte)
{
    for (int i = 7; i >= 0; i--) {
        if (byte & (1 << i)) SDA_HIGH();
        else                 SDA_LOW();
        i2c_delay();
        SCL_HIGH(); i2c_delay();
        SCL_LOW();  i2c_delay();
    }
    // Clock ACK (bỏ qua, không đọc)
    SDA_HIGH(); i2c_delay();
    SCL_HIGH(); i2c_delay();
    SCL_LOW();  i2c_delay();
}

static void i2c_transmit(uint8_t addr, uint8_t *data, uint8_t len)
{
    i2c_start();
    i2c_write_byte(addr << 1); // địa chỉ 7-bit + write bit
    for (uint8_t i = 0; i < len; i++) {
        i2c_write_byte(data[i]);
    }
    i2c_stop();
}

/* ============================================================================
 * LCD DRIVER
 * ============================================================================ */

void lcd_send_cmd(char cmd)
{
    char data_u = (cmd & 0xf0);
    char data_l = ((cmd << 4) & 0xf0);
    uint8_t data_t[4] = {
        data_u | 0x0C,
        data_u | 0x08,
        data_l | 0x0C,
        data_l | 0x08
    };
    i2c_transmit(SLAVE_ADDRESS_LCD, data_t, 4);
}

void lcd_send_data(char data)
{
    char data_u = (data & 0xf0);
    char data_l = ((data << 4) & 0xf0);
    uint8_t data_t[4] = {
        data_u | 0x0D,
        data_u | 0x09,
        data_l | 0x0D,
        data_l | 0x09
    };
    i2c_transmit(SLAVE_ADDRESS_LCD, data_t, 4);
}

void lcd_init(void)
{
    // Cấu hình PA0, PA1 là output open-drain (chuẩn I2C)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin   = LCD_SCL_PIN | LCD_SDA_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    SDA_HIGH(); SCL_HIGH();
    HAL_Delay(50);

    lcd_send_cmd(0x33);
    lcd_send_cmd(0x32); HAL_Delay(50);
    lcd_send_cmd(0x28); HAL_Delay(50); // 4-bit, 2 line
    lcd_send_cmd(0x01); HAL_Delay(50); // clear
    lcd_send_cmd(0x06); HAL_Delay(50); // entry mode
    lcd_send_cmd(0x0C); HAL_Delay(50); // display on
    lcd_send_cmd(0x02); HAL_Delay(50); // cursor home
    lcd_send_cmd(0x80);
}

void lcd_send_string(char *str)
{
    while (*str) lcd_send_data(*str++);
}

void lcd_clear_display(void)
{
    lcd_send_cmd(0x01);
    HAL_Delay(10);
}

void lcd_goto_XY(int row, int col)
{
    uint8_t pos_Addr;
    if (row == 1) pos_Addr = 0x80 + col;
    else          pos_Addr = 0x80 | (0x40 + col);
    lcd_send_cmd(pos_Addr);
}

/* ============================================================================
 * HIỂN THỊ THÔNG TIN HỆ THỐNG
 *
 * Row 1 (normal): "RUN  R:05 G:03  "
 * Row 1 (adjust): "ADJ-R SET:07    "
 * Row 2:          "Rd1:05  Rd2:03  "
 * ============================================================================ */
static const char* get_mode_name(void)
{
    switch (current_mode) {
    case MODE_1_NORMAL:       return "RUN  ";
    case MODE_2_RED_MODIFY:   return "ADJ-R";
    case MODE_3_AMBER_MODIFY: return "ADJ-Y";
    case MODE_4_GREEN_MODIFY: return "ADJ-G";
    default:                  return "?????";
    }
}

void lcd_update_display(void)
{
    char buf[17];

    lcd_goto_XY(1, 0);
    if (current_mode == MODE_1_NORMAL)
        snprintf(buf, sizeof(buf), "%s R:%02d G:%02d  ", get_mode_name(), duration_RED, duration_GREEN);
    else
        snprintf(buf, sizeof(buf), "%s SET:%02d      ", get_mode_name(), temp_duration);
    lcd_send_string(buf);

    lcd_goto_XY(2, 0);
    snprintf(buf, sizeof(buf), "Rd1:%02d  Rd2:%02d  ", counter_road1, counter_road2);
    lcd_send_string(buf);
}
