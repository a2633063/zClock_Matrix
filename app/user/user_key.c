#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"
#include "espconn.h"

#include "../cJson/cJSON.h"
#include "driver\key.h"
#include "user_key.h"
#include "user_led.h"
#include "user_wifi.h"
#include "user_json.h"
#include "user_setting.h"

LOCAL struct keys_param keys;
LOCAL struct single_key_param *single_key[GPIO_KEY_NUM];

LOCAL unsigned char key_press_flag = 0;	//按键长按标志位,防止按键长按后松开时执行短按代码

void ICACHE_FLASH_ATTR
user_relay_set( char level) {
	if (level != -1) {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELAY_IO_NUM), level);
	} else {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELAY_IO_NUM), !GPIO_INPUT_GET(GPIO_ID_PIN(GPIO_RELAY_IO_NUM)));
	}
	user_set_led_wifi(user_config.on);
}

LOCAL void ICACHE_FLASH_ATTR
user_key_short_press(void) {
	uint8_t i, result = 1;
	char strJson[128];
	os_printf("user_key_short_press\n");
	if (key_press_flag == 1) {	//防止按键长按后松开时执行短按代码
		key_press_flag = 0;
		return;
	}

	user_config.on = !user_config.on;

	os_sprintf(strJson, "{\"mac\":\"%s\",\"on\":%d}", strMac, user_config.on);
	user_json_analysis(false, strJson);
}

LOCAL void ICACHE_FLASH_ATTR
user_key_long_press(void) {
	os_printf("user_key_long_press\n");
	key_press_flag = 1;
}

//LOCAL void ICACHE_FLASH_ATTR
//user_key_long_10s_press(void) {
//	os_printf("user_key_long_10s_press\n");
////恢复出厂设置
//	user_config.name[0] = 0xff;
//	user_config.name[1] = 0xff;
//	user_config.name[2] = 0xff;
//	user_config.name[3] = 0;
//	user_setting_set_config();
//	system_restore();
//}


void ICACHE_FLASH_ATTR
user_key_init(void) {

	PIN_FUNC_SELECT(GPIO_RELAY_IO_MUX, GPIO_RELAY_IO_FUNC);
	single_key[0] = key_init_single(GPIO_KEY_0_IO_NUM, GPIO_KEY_0_IO_MUX,
	GPIO_KEY_0_IO_FUNC, user_key_long_press, user_key_short_press);

	keys.key_num = GPIO_KEY_NUM;
	keys.single_key = single_key;
	key_init(&keys);

}
