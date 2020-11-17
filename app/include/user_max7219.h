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
void user_max7219_dis_char(char ch, int8_t x, int8_t y);
void user_max7219_dis_str(char * ch, int8_t x, int8_t y);
void user_max7219_dis_num(char ch, int8_t x, int8_t y);
void user_max7219_dis_num_small(char ch, int8_t x, int8_t y);
#endif
