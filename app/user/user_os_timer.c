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
uint8_t * string_p = NULL;
struct struct_time time_last;
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
	uint8_t dis_refresh_flag = 0;
	int16_t dis_scroll_temp;

	if (++timer_num > 15) {
		timer_num = 0;
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
		user_max7219_dis_char('l', 11, timer_count > 8 ? 1 : timer_count - 7);
		user_max7219_dis_str("zC", 0, timer_count > 8 ? 1 : timer_count - 7);
		user_max7219_dis_str("oc", 16, timer_count > 8 ? 1 : timer_count - 7);
		user_max7219_dis_char('k', 27, timer_count > 8 ? 1 : timer_count - 7);
//		user_max7219_clear(0);
		dis_refresh_flag = 1;
		if (timer_count > 40) {

			if (wifi_get_opmode() == STATION_MODE) {
				os_printf("DISPLAY_STATE_WIFI_CONNECTING\n");
				user_set_display_state(DISPLAY_STATE_WIFI_CONNECTING);
			} else {
				os_printf("DISPLAY_STATE_WIFI_AP\n");
				user_set_display_state(DISPLAY_STATE_WIFI_AP);
			}
		}
		break;
	case DISPLAY_STATE_WIFI_CONNECTING:
		timer_count++;
		if (display_state_last == DISPLAY_STATE_INIT) {
			//上滚动部分
			if (timer_count < 7) {
				for (i = 0; i < 8 - timer_count; i++) {
					for (j = 0; j < 4; j++)
						display[j][i] = display[j][i + 1];
				}
			}
			i = (timer_count > 8) ? (0) : (8 - timer_count);
			for (; i < 8; i++) {
				for (j = 0; j < 4; j++)
					display[j][i] = 0;
			}
			user_max7219_dis_char('W', 3, 11 - timer_count);
			user_max7219_dis_char('i', 8, 11 - timer_count);
			user_max7219_dis_char('-', 13, 11 - timer_count);
			user_max7219_dis_char('F', 19, 11 - timer_count);
			user_max7219_dis_char('i', 24, 11 - timer_count);
//			user_max7219_dis_str("Wi-Fi", 1, 11 - timer_count);

			if (timer_count == 11) {
				display_state_last = display_state;
				timer_count = 0;
			}
			dis_refresh_flag = 1;
			break;
		}

		if (timer_count == 1) {
			display[0][7] = 0x80;
		} else if (timer_count > 1) {
			for (j = 3; j > 0; j--)
				display[j][7] = (display[j][7] >> 1) | (display[j - 1][7] << 7);

			display[0][7] = display[0][7] >> 1;

			if (timer_count == 35) {
				timer_count = 0;
				i = wifi_station_get_connect_status();
				switch (i) {
				case STATION_CONNECTING:
					break;
				case STATION_WRONG_PASSWORD:
				case STATION_NO_AP_FOUND:
				case STATION_CONNECT_FAIL:
					user_set_display_state(DISPLAY_STATE_ERR);
					break;
				case STATION_GOT_IP:
					user_set_display_state(DISPLAY_STATE_OK);
					break;
				}
			}
		}
		dis_refresh_flag = 1;
		break;
	case DISPLAY_STATE_OK:
		timer_count++;
		if (display_state_last != display_state) {
			//上滚动部分
			if (timer_count < 7) {
				for (i = 0; i < 8 - timer_count; i++) {
					for (j = 0; j < 4; j++)
						display[j][i] = display[j][i + 1];
				}
			}
			i = (timer_count > 8) ? (0) : (8 - timer_count);
			for (; i < 8; i++) {
				for (j = 0; j < 4; j++)
					display[j][i] = 0;
			}
			user_max7219_dis_char('O', 6, 11 - timer_count);
			user_max7219_dis_char('K', 12, 11 - timer_count);
			user_max7219_dis_char('!', 19, 11 - timer_count);
			user_max7219_dis_char('!', 25, 11 - timer_count);
//			user_max7219_dis_str("Wi-Fi", 3, 11 - timer_count);

			if (timer_count == 11) {
				display_state_last = display_state;
				timer_count = 0;
			}
			dis_refresh_flag = 1;
			break;
		}

		if (timer_count > 40)
			user_set_display_state(DISPLAY_STATE_TIME);
//		dis_refresh_flag = 1;
		break;
	case DISPLAY_STATE_ERR:
		timer_count++;
		if (display_state_last != display_state) {
			//上滚动部分
			if (timer_count < 7) {
				for (i = 0; i < 8 - timer_count; i++) {
					for (j = 0; j < 4; j++)
						display[j][i] = display[j][i + 1];
				}
			}
			i = (timer_count > 8) ? (0) : (8 - timer_count);
			for (; i < 8; i++) {
				for (j = 0; j < 4; j++)
					display[j][i] = 0;
			}
			user_max7219_dis_char('E', 6, 11 - timer_count);
			user_max7219_dis_char('r', 12, 11 - timer_count);
			user_max7219_dis_char('r', 19, 11 - timer_count);
			user_max7219_dis_char('!', 25, 11 - timer_count);
			//			user_max7219_dis_str("Wi-Fi", 3, 11 - timer_count);

			if (timer_count == 11) {
				display_state_last = display_state;
				timer_count = 0;
			}
			dis_refresh_flag = 1;
			break;
		}

		if (timer_count > 40)
			user_set_display_state(DISPLAY_STATE_AP_CONFIRM);
//		dis_refresh_flag = 1;
		break;
	case DISPLAY_STATE_AP_CONFIRM:

		if (display_state_last != display_state) {
			timer_count++;
			//上滚动部分
			if (timer_count < 7) {
				for (i = 0; i < 8 - timer_count; i++) {
					for (j = 0; j < 4; j++)
						display[j][i] = display[j][i + 1];
				}
			}
			i = (timer_count > 8) ? (0) : (8 - timer_count);
			for (; i < 8; i++) {
				for (j = 0; j < 4; j++)
					display[j][i] = 0;
			}
			user_max7219_dis_char('A', 6, 11 - timer_count);
			user_max7219_dis_char('P', 12, 11 - timer_count);
			user_max7219_dis_char('?', 19, 11 - timer_count);
			user_max7219_dis_char('?', 25, 11 - timer_count);
			//			user_max7219_dis_str("Wi-Fi", 1, 11 - timer_count);

			if (timer_count == 11) {
				display_state_last = display_state;
				timer_count = 0;
			}
			dis_refresh_flag = 1;
			break;
		}

		if (!user_key_read()) {
			//按钮按下
			os_printf("DISPLAY_STATE_AP_CONFIRM: key pressed\n");
			user_set_display_state(DISPLAY_STATE_AP_WEB);
			display[0][7] = 0;
			display[1][7] = 0;
			display[2][7] = 0;
			display[3][7] = 0;
			dis_refresh_flag = 1;
			user_wifi_AP();
			break;
		}
		if (timer_num % 4 == 0) {
			timer_count++;

			if (timer_count < 33) {
				display[(timer_count - 1) / 8][7] = display[(timer_count - 1) / 8][7] >> 1 | 0x80;
			} else {

				display[0][7] = 0;
				display[1][7] = 0;
				display[2][7] = 0;
				display[3][7] = 0;

				user_set_display_state(DISPLAY_STATE_TIME);
			}

			dis_refresh_flag = 1;
		}
		break;
	case DISPLAY_STATE_AP_WEB:
		timer_count++;
		if (display_state_last != display_state) {
			//上滚动部分
			if (timer_count < 7) {
				for (i = 0; i < 8 - timer_count; i++) {
					for (j = 0; j < 4; j++)
						display[j][i] = display[j][i + 1];
				}
			}
			i = (timer_count > 8) ? (0) : (8 - timer_count);
			for (; i < 8; i++) {
				for (j = 0; j < 4; j++)
					display[j][i] = 0;
			}
			user_max7219_dis_char('A', 0, 11 - timer_count);
			user_max7219_dis_char('P', 6, 11 - timer_count);
			user_max7219_dis_char('W', 15, 11 - timer_count);
			user_max7219_dis_char('e', 21, 11 - timer_count);
			user_max7219_dis_char('b', 27, 11 - timer_count);

			if (timer_count == 11) {
				display_state_last = display_state;
				timer_count = 0;
			}
			dis_refresh_flag = 1;
			break;
		}

//		if (timer_count > 40)
//			user_set_display_state(DISPLAY_STATE_AP_CONFIRM);
//		dis_refresh_flag = 1;
		break;
	case DISPLAY_STATE_TIME:
		if (display_state_last != display_state) {
			//上滚动部分
			timer_count++;
			if (timer_count < 7) {
				for (i = 0; i < 8 - timer_count; i++) {
					for (j = 0; j < 4; j++)
						display[j][i] = display[j][i + 1];
				}
			}
			i = (timer_count > 8) ? (0) : (8 - timer_count);
			for (; i < 8; i++) {
				for (j = 0; j < 4; j++)
					display[j][i] = 0;
			}

			user_max7219_dis_num(':', 9, 11 - timer_count);
			user_max7219_dis_num(time.hour / 10, 0, 11 - timer_count);
			user_max7219_dis_num(time.hour % 10, 5, 11 - timer_count);
			user_max7219_dis_num(time.minute / 10, 14, 11 - timer_count);
			user_max7219_dis_num(time.minute % 10, 19, 11 - timer_count);
			user_max7219_dis_num_small(time.second / 10, 25, 13 - timer_count);
			user_max7219_dis_num_small(time.second % 10, 29, 13 - timer_count);
			time_last = time;
			if (timer_count == 11) {
				display_state_last = display_state;
				timer_count = 0;
			}

			dis_refresh_flag = 1;
			break;
		}

//		user_max7219_dis_num(':', 9, 0);
		//user_max7219_dis_num(time.hour / 10, 0, 0);
		//user_max7219_dis_num(time.minute / 10, 14, 0);
		//user_max7219_dis_num(time.minute % 10, 19, 0);
		//user_max7219_dis_num_small(time.second / 10, 25, 2);
		//user_max7219_dis_num_small(time.second % 10, 29, 2);
		if (time_last.second != time.second || time_last.minute != time.minute || time_last.hour != time.hour) {
			timer_count++;
			if (time_last.hour / 10 != time.hour / 10) {
				user_max7219_dis_scroll_num(time_last.hour / 10, time.hour / 10, timer_count - 1, 0, 0);
			}

			if (time_last.hour % 10 != time.hour % 10) {
				//user_max7219_dis_num(time.hour % 10, 5, 0);
				user_max7219_dis_scroll_num(time_last.hour % 10, time.hour % 10, timer_count - 1, 5, 0);

				if (timer_count > 9)
					time_last.hour = time.hour;
			}

			if (time_last.minute / 10 != time.minute / 10) {
				user_max7219_dis_scroll_num(time_last.minute / 10, time.minute / 10, timer_count - 1, 14, 0);
			}

			if (time_last.minute % 10 != time.minute % 10) {
				user_max7219_dis_scroll_num(time_last.minute % 10, time.minute % 10, timer_count - 1, 19, 0);

				if (timer_count > 9) {
					time_last.minute = time.minute;
				}
			}

			if (time_last.second / 10 != time.second / 10) {
				user_max7219_dis_scroll_num_small(time_last.second / 10, time.second / 10, timer_count - 1, 25, 2);
			}

			if (time_last.second % 10 != time.second % 10) {
				user_max7219_dis_scroll_num_small(time_last.second % 10, time.second % 10, timer_count - 1, 29, 2);
				if (timer_count > 7) {
					time_last.second = time.second;
				}
			}
			dis_refresh_flag = 1;

		} else
			timer_count = 0;

		break;
	case DISPLAY_STATE_STRING:
		if (display_state_last != display_state) {
			timer_count++;
			//下滚动部分
			if (timer_count < 9) {
				for (i = 7; i > 0; i--) {
					for (j = 0; j < 4; j++)
						display[j][i] = display[j][i - 1];
				}
				for (j = 0; j < 4; j++)
					display[j][0] = 0;
			}

			if (timer_count == 8) {
				display_state_last = display_state;
				timer_count = 0;
			}
			dis_refresh_flag = 1;
			break;
		}
		if (string_p == NULL) {
			user_set_display_state(DISPLAY_STATE_TIME);
			break;
		}
		if (timer_num % 3 == 0) {
			timer_count++;
			user_max7219_clear(0);
			if (timer_count < os_strlen(string_p) * 6 + 32) {
				user_max7219_dis_str(string_p, 32 - timer_count, 0);
				dis_refresh_flag = 1;
			} else {
				if (string_p != NULL) {
					os_free(string_p);
					string_p = NULL;
				}
				user_set_display_state(DISPLAY_STATE_TIME);
			}
		}
		break;
	}

	if (dis_refresh_flag == 1)
		user_max7219_dis_refresh();

}

void ICACHE_FLASH_ATTR
user_set_display_string(uint8_t *p) {
	user_set_display_state(DISPLAY_STATE_STRING);
	if (string_p != NULL) {
		os_free(string_p);
		string_p = NULL;
	}
	string_p = p;
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
	os_timer_arm(&timer_rtc, 25, 1);
}
