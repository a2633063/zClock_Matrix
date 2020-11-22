#ifndef __USER_OS_TIMER_H__
#define __USER_OS_TIMER_H__

void ICACHE_FLASH_ATTR user_os_timer_init(void);

void user_set_display_state(display_state_t s) ;
void user_set_display_string(uint8_t *p);
#endif

