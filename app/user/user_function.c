#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "espconn.h"

#include "user_wifi.h"
#include "user_udp.h"
#include "user_mqtt.h"
#include "user_function.h"
#include "user_setting.h"

void ICACHE_FLASH_ATTR
user_send(bool udp_flag, uint8_t *s, char retained) {
	if (udp_flag || !user_mqtt_is_connect()) {
		user_udp_send(s);
	} else {
		user_mqtt_send(s, 1, retained);
	}
}

void ICACHE_FLASH_ATTR
user_con_received(void *arg, char *pusrdata, unsigned short length) {
	if (length == 1 && *pusrdata == 127)
		return;

	struct espconn *pesp_conn = arg;

	user_json_analysis(true, pusrdata);

}


LOCAL os_timer_t function_restart_timer; //timer,一段时间后重启设备
void user_function_restart_timer_fun(void *arg) {
	system_restart();
}
void ICACHE_FLASH_ATTR
user_function_restart(uint32_t t) {
	os_timer_disarm(&function_restart_timer);
	os_timer_setfn(&function_restart_timer, (os_timer_func_t *) user_function_restart_timer_fun, NULL);
	os_timer_arm(&function_restart_timer, t, 0); //一段时间后重启
}

