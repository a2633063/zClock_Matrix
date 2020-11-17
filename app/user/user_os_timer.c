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

#include "sntp.h"
#include "user_config.h"
#include "../cJson/cJSON.h"
#include "user_key.h"
#include "user_os_timer.h"
#include "user_wifi.h"
#include "user_sntp.h"
#include "user_max7219.h"
#include "user_ds3231.h"

uint32_t utc_time = 0;

struct struct_time time;
uint32_t current_stamp = 0;
uint8_t timeBCD[7];
LOCAL os_timer_t timer_rtc;
uint32_t timer_count = 0;
display_state_t display_state_last = DISPLAY_STATE_INIT;
void ICACHE_FLASH_ATTR user_os_timer_func(void *arg) {

//	uint8_t DeviceBuffer[28] = { 0 };
//	int8_t task_flag = -1;   //记录每个插座哪个任务需要返回数据
//	uint8_t i, j;

//	if (utc_time == 0 || (time.second == 59 && time.minute == 59)) { //每小时校准一次
//		if (wifi_station_get_connect_status() == STATION_GOT_IP) {
//			utc_time = sntp_get_current_timestamp();
//		}
//	}
//
//	if (utc_time > 0) {
//		utc_time++;
//		os_sprintf(DeviceBuffer, "%s", sntp_get_real_time(utc_time));
//		time_strtohex(&time,DeviceBuffer);
//
//		if (time.second == 0)
//			os_printf("20%02d/%02d/%02d 周%d %02d:%02d:%02d\n", time.year, time.month, time.day, time.week, time.hour, time.minute, time.second);
//
//		bool update_user_config_flag = false;
//	}
	static uint8_t timer_num = 0;
	uint32_t current_stamp_temp;
	uint8_t i, j;
	int16_t dis_scroll_temp;

	if (++timer_num > 4) {
		if ((current_stamp == 0 || (time.hour == 4 && time.minute == 00 && time.second == 20))
				&& wifi_station_get_connect_status() == STATION_GOT_IP) {
			current_stamp_temp = sntp_get_current_timestamp();
			if (current_stamp_temp > 0) {
				current_stamp = current_stamp_temp;

				time_strtohex(&time, (char*) (sntp_get_real_time(current_stamp)));
				os_printf("SNTP : %d \n", current_stamp);
				timeBCD[0] = DECtoBCD(time.second);
				timeBCD[1] = DECtoBCD(time.minute);
				timeBCD[2] = DECtoBCD(time.hour);
				timeBCD[3] = DECtoBCD(time.week);
				timeBCD[4] = DECtoBCD(time.day);
				timeBCD[5] = DECtoBCD(time.month);
				timeBCD[6] = DECtoBCD(time.year);
				user_ds3231_page_write(0, timeBCD, 7);	//时间写入ds3231
				os_printf("SNTP to ds3231 \n");
			} else {
				os_printf("SNTP : fail \n");
			}
		}

		user_ds3231_page_read(0, timeBCD, 7);
		time.second = BCDtoDEC(timeBCD[0]);
		time.minute = BCDtoDEC(timeBCD[1]);
		time.hour = BCDtoDEC(timeBCD[2]);
		time.week = BCDtoDEC(timeBCD[3]);
		time.day = BCDtoDEC(timeBCD[4]);
		time.month = BCDtoDEC(timeBCD[5]);
		time.year = BCDtoDEC(timeBCD[6]);

//	if (time.second == 0)
//		os_printf("20%02d/%02d/%02d 周%d %02d:%02d:%02d\n", time.year, time.month, time.day, time.week, time.hour, time.minute, time.second);
	}
	switch (display_state) {
	case DISPLAY_STATE_INIT:
		timer_count++;

		user_max7219_clear(0);
		user_max7219_dis_char('l', 11, timer_count > 8 ? 0 : timer_count - 7);
		user_max7219_dis_str("zC", 0, timer_count > 8 ? 0 : timer_count - 7);
		user_max7219_dis_str("oc", 16, timer_count > 8 ? 0 : timer_count - 7);
		user_max7219_dis_char('k', 27, timer_count > 8 ? 0 : timer_count - 7);

		if (timer_count > 14) {
			display_state = DISPLAY_STATE_TIME;
			timer_count = 0;
		}
		break;

	case DISPLAY_STATE_TIME:
		if (display_state_last != display_state) {
			for (i = 0; i < 7; i++) {
				for (j = 0; j < 4; j++)
					display[j][i] = display[j][i + 1];
			}
		}
		display[1][1] = 0x10;
		display[1][2] = 0x10;
		display[1][4] = 0x10;
		display[1][5] = 0x10;
		//	user_max7219_dis_num(':',11,0);
		user_max7219_dis_num(time.hour / 10, 0, 0);
		user_max7219_dis_num(time.hour % 10, 5, 0);
		user_max7219_dis_num(time.minute / 10, 14, 0);
		user_max7219_dis_num(time.minute % 10, 19, 0);
		user_max7219_dis_num_small(time.second / 10, 25, 2);
		user_max7219_dis_num_small(time.second % 10, 29, 2);
		break;
	}

}

void ICACHE_FLASH_ATTR
user_set_display_state(display_state_t s) {
	display_state_last = display_state;
	display_state = s;
	timer_count = 0;
}
void ICACHE_FLASH_ATTR
user_os_timer_init(void) {
	os_timer_disarm(&timer_rtc);
	os_timer_setfn(&timer_rtc, (os_timer_func_t *) user_os_timer_func, NULL);
	os_timer_arm(&timer_rtc, 100, 1);
}
