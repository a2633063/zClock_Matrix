/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2015/1/23, v1.0 create this file.
 *******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"

#include "user_config.h"
#include "../cJson/cJSON.h"
#include "user_key.h"
#include "user_wifi.h"
#include "user_sntp.h"

LOCAL os_timer_t timer_rtc;

uint32 utc_time = 0;
void ICACHE_FLASH_ATTR user_os_timer_func(void *arg) {

	uint8_t DeviceBuffer[28] = { 0 };
	int8_t task_flag = -1;   //��¼ÿ�������ĸ�������Ҫ��������
	uint8_t i, j;

	if (utc_time == 0 || (time.second == 59 && time.minute == 59)) { //ÿСʱУ׼һ��
		if (wifi_station_get_connect_status() == STATION_GOT_IP) {
			utc_time = sntp_get_current_timestamp();
		}
	}

	if (utc_time > 0) {
		utc_time++;
		os_sprintf(DeviceBuffer, "%s", sntp_get_real_time(utc_time));
		time_strtohex(DeviceBuffer);

		if (time.second == 0)
			os_printf("20%02d/%02d/%02d ��%d %02d:%02d:%02d\n", time.year, time.month, time.day, time.week, time.hour, time.minute, time.second);

		bool update_user_config_flag = false;
		//TODO �޸Ķ�ʱ�����߼�

		for (j = 0; j < TIME_TASK_NUM; j++) {
			if (user_config.task[j].on != 0) {

				uint8_t repeat = user_config.task[j].repeat;
				if (    //����������ı�̵���״̬: ��Ϊ0 ʱ�ַ����趨ֵ, �ظ������趨ֵ
				time.second == 0 && time.minute == user_config.task[j].minute && time.hour == user_config.task[j].hour
						&& ((repeat == 0x00) || repeat & (1 << (time.week - 1)))) {
					if (user_config.on != user_config.task[j].action) {
						user_config.on = user_config.task[j].action;
						update_user_config_flag = true;
					}
					if (repeat == 0x00) {
						task_flag = j;
						user_config.task[j].on = 0;
						update_user_config_flag = true;
					}
				}
			}

		}

		//���´������� ���¶�ʱ��������
		if (update_user_config_flag == true) {
			os_printf("update_user_config_flag");
			update_user_config_flag = false;

//			user_io_set_plug_all(2, 2, 2, 2);
//			user_setting_set_config();
//			cJSON *json_send = cJSON_CreateObject();
//			cJSON_AddStringToObject(json_send, "mac", strMac);
//			cJSON_AddNumberToObject(json_send, "on", user_config.on);
//			if (task_flag >= 0) {
//				j = task_flag;
//				char strTemp2[] = "task_X";
//				strTemp2[5] = j + '0';
//				cJSON *json_send_plug_task = cJSON_CreateObject();
//				cJSON_AddNumberToObject(json_send_plug_task, "hour", user_config.task[j].hour);
//				cJSON_AddNumberToObject(json_send_plug_task, "minute", user_config.task[j].minute);
//				cJSON_AddNumberToObject(json_send_plug_task, "repeat", user_config.task[j].repeat);
//				cJSON_AddNumberToObject(json_send_plug_task, "action", user_config.task[j].action);
//				cJSON_AddNumberToObject(json_send_plug_task, "on", user_config.task[j].on);
//				cJSON_AddItemToObject(json_send, strTemp2, json_send_plug_task);
//				task_flag = -1;
//			}
//			char *json_str = cJSON_Print(json_send);
//			user_send( false, json_str);    //��������

//			os_free(json_str);
//			cJSON_Delete(json_send);
			//            os_printf("cJSON_Delete");

			char strJson[128];
			os_sprintf(strJson, "{\"mac\":\"%s\",\"on\":%d}", strMac, user_config.on);
			user_json_analysis(false, strJson);
		}

	}

}

void ICACHE_FLASH_ATTR
user_os_timer_init(void) {
	os_timer_disarm(&timer_rtc);
	os_timer_setfn(&timer_rtc, (os_timer_func_t *) user_os_timer_func, NULL);
	os_timer_arm(&timer_rtc, 1000, 1);
}
