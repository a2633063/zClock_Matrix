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

	int i, j;
	uint32_t k;

	user_json_analysis(true, pusrdata);

}
