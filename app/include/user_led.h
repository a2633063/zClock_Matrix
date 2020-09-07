#ifndef __USER_LED_H__
#define __USER_LED_H__

#include "gpio.h"


#define GPIO_LED_WIFI_IO_MUX     PERIPHS_IO_MUX_MTDI_U
#define GPIO_LED_WIFI_IO_NUM     12
#define GPIO_LED_WIFI_IO_FUNC    FUNC_GPIO12

void user_led_init(void);

void user_set_led_wifi(int8_t level);


#endif
