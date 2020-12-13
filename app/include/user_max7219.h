#ifndef __USER_LED_H__
#define __USER_LED_H__

#include "gpio.h"

#define MAX7219_CS_IO_MUX     PERIPHS_IO_MUX_MTDO_U
#define MAX7219_CS_IO_NUM     15
#define MAX7219_CS_IO_FUNC    FUNC_GPIO15

extern uint8_t display[4][8];
extern uint8_t brightness_on;
extern uint8_t brightness;
void user_max7219_init(void);
void user_max7219_clear(uint8_t dat);
void user_max7219_set_brightness(uint8_t bri);
void user_max7219_set_on(uint8_t on);
void user_max7219_dis_char(char ch, int16_t x, int16_t y);
void user_max7219_dis_str(char * ch, int16_t x, int16_t y);
void user_max7219_dis_num(char ch, int16_t x, int16_t y);
void user_max7219_dis_num_small(char ch, int16_t x, int16_t y);
void user_max7219_dis_refresh(void);
void user_max7219_dis_scroll_num(uint8_t up_val, uint8_t down_val, int8_t scroll, int8_t x, int8_t y);
void user_max7219_dis_scroll_num_small(uint8_t up_val, uint8_t down_val, int8_t scroll, int8_t x, int8_t y);

#endif
