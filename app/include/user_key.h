#ifndef __USER_KEY_H__
#define __USER_KEY_H__

#define GPIO_KEY_NUM            1

#define GPIO_RELAY_IO_MUX     PERIPHS_IO_MUX_MTDO_U
#define GPIO_RELAY_IO_NUM     15
#define GPIO_RELAY_IO_FUNC    FUNC_GPIO15


#define GPIO_KEY_0_IO_MUX     PERIPHS_IO_MUX_MTCK_U
#define GPIO_KEY_0_IO_NUM     13
#define GPIO_KEY_0_IO_FUNC    FUNC_GPIO13

void user_key_init(void);
void user_relay_set( char level);
#endif
