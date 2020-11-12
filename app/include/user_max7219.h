#ifndef __USER_LED_H__
#define __USER_LED_H__

#include "gpio.h"

#define MAX7219_CS_IO_MUX     PERIPHS_IO_MUX_MTDO_U
#define MAX7219_CS_IO_NUM     15
#define MAX7219_CS_IO_FUNC    FUNC_GPIO15

void user_max7219_init(void);
void user_max7219_clear(void);
void user_max7219_set_brightness(uint8 bri);
#endif
