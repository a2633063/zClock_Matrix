#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"
#include "espconn.h"

#include "../cJson/cJSON.h"
#include "driver\key.h"
#include "user_key.h"
#include "user_max7219.h"
#include "user_wifi.h"
#include "user_json.h"
#include "user_setting.h"

LOCAL struct keys_param keys;
LOCAL struct single_key_param *single_key[GPIO_KEY_NUM];

LOCAL unsigned char key_press_flag = 0;	//����������־λ,��ֹ�����������ɿ�ʱִ�ж̰�����

LOCAL void ICACHE_FLASH_ATTR
user_key_short_press(void) {
	uint8_t i, result = 1;
	char strJson[128];
	os_printf("user_key_short_press\n");
	if (key_press_flag == 1) {	//��ֹ�����������ɿ�ʱִ�ж̰�����
		key_press_flag = 0;
		return;
	}

//	user_config.on = !user_config.on;

	os_sprintf(strJson, "{\"mac\":\"%s\",\"on\":%d}", strMac, user_config.on);
	user_json_analysis(false, strJson);
}

LOCAL void ICACHE_FLASH_ATTR
user_key_long_press(void) {
	os_printf("user_key_long_press\n");
	key_press_flag = 1;
	char strJson[128];
	user_config.direction = !user_config.direction;

	os_sprintf(strJson, "{\"mac\":\"%s\",\"direction\":%d}", strMac, user_config.direction);
	user_json_analysis(false, strJson);

}

//LOCAL void ICACHE_FLASH_ATTR
//user_key_long_10s_press(void) {
//	os_printf("user_key_long_10s_press\n");
////�ָ���������
//	user_config.name[0] = 0xff;
//	user_config.name[1] = 0xff;
//	user_config.name[2] = 0xff;
//	user_config.name[3] = 0;
//	user_setting_set_config();
//	system_restore();
//}

void ICACHE_FLASH_ATTR
user_key_init(void) {

	single_key[0] = key_init_single(GPIO_KEY_0_IO_NUM, GPIO_KEY_0_IO_MUX,
	GPIO_KEY_0_IO_FUNC, user_key_long_press, user_key_short_press);

	keys.key_num = GPIO_KEY_NUM;
	keys.single_key = single_key;
	key_init(&keys);

}

uint8_t ICACHE_FLASH_ATTR
user_key_read(void) {
	return GPIO_INPUT_GET(GPIO_ID_PIN(GPIO_KEY_0_IO_NUM));
}
