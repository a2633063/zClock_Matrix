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
#include "mem.h"

#include "user_interface.h"
#include "espconn.h"

#include "user_sntp.h"
#include "user_ds3231.h"

struct struct_time time;
uint32 current_stamp = 0;
uint8 timeBCD[7];
LOCAL os_timer_t timer_sntp;
uint8 user_sntp_timer_count = 0;

void ICACHE_FLASH_ATTR user_sntp_timer_func(void *arg) {
	uint32 current_stamp_temp;
	uint8 i;
	user_sntp_timer_count++;
	if ((current_stamp == 0 || (time.second == 10 && time.minute == 59)) && wifi_station_get_connect_status() == STATION_GOT_IP) {
		current_stamp_temp = sntp_get_current_timestamp();
		if (current_stamp_temp > 0) {
			current_stamp = current_stamp_temp;

			time_strtohex((char*) (sntp_get_real_time(current_stamp)));
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

	os_printf("20%02d/%02d/%02d 周%d %02d:%02d:%02d\n", time.year, time.month, time.day, time.week, time.hour, time.minute, time.second);
//	user_alarm_check(time);
//	if (auto_brightness != 0) {
//		uint16 adc = system_adc_read();
//		if (adc > BrightnessVal[0]) {
//			brightness=1;
//		}else if (adc >BrightnessVal[1] ) {
//			brightness=2;
//		}else if (adc >BrightnessVal[2] ) {
//			brightness=3;
//		}else if (adc >BrightnessVal[3] ) {
//			brightness=4;
//		}else if (adc >BrightnessVal[4] ) {
//			brightness=5;
//		}else if (adc > BrightnessVal[5]) {
//			brightness=6;
//		}else if (adc > BrightnessVal[6]) {
//			brightness=7;
//		}else {
//			brightness=8;
//		}
//		os_printf("brightness:%d		ADC : %d \n",brightness, adc);
//	}

//	if(user_sntp_timer_count<20){
//		user_tm1628_time_refresh(0);
//	}else if(user_sntp_timer_count<21){
//		user_tm1628_time_refresh(1);
//	}else{//bug:当user_sntp_timer_count==11时 不刷新显示,显示为日期
//		user_sntp_timer_count=0;
//	}

}

void ICACHE_FLASH_ATTR
user_sntp_init(void) {
	sntp_set_timezone(8);	//时区:+8
	ip_addr_t *addr = (ip_addr_t *) os_zalloc(sizeof(ip_addr_t));
	ipaddr_aton("210.72.145.44", addr);
	sntp_setserver(0, addr); // set server 0 by IP address
	sntp_setservername(1, "us.pool.ntp.org"); // set server 1 by domain name
	sntp_setservername(2, "ntp.sjtu.edu.cn"); // set server 2 by domain name

	sntp_init();
	os_free(addr);
	current_stamp = 0;

	os_timer_disarm(&timer_sntp);
	os_timer_setfn(&timer_sntp, (os_timer_func_t *) user_sntp_timer_func, NULL);
	os_timer_arm(&timer_sntp, 400, 1);	//400ms
}

// 将sntp_get_real_time获取到的真实时间字符串,转换为变量time
void ICACHE_FLASH_ATTR time_strtohex(char* sntp_time) {
//获取星期
//返回内容为英文Mon,Tues,Wed,Thur,Fri,Sat,Sun 比较第2个字母(周二与周日相同比较第一个)
	switch (sntp_time[1]) {
	case 'o':
		time.week = Monday;
		break;	//周一
	case 'e':
		time.week = Wednesday;
		break;	//周三
	case 'h':
		time.week = Thursday;
		break;	//周四
	case 'r':
		time.week = Friday;
		break;	//周五
	case 'a':
		time.week = Saturday;
		break;	//周六
	case 'u':
		if (sntp_time[0] == 'S')
			time.week = Sunday;		//周日
		else if (sntp_time[0] == 'T')
			time.week = Tuesday;		//周二
		break;
	}

//获取英文
//比较第3个字母
	sntp_time = (char *) os_strstr(sntp_time, " ");
	sntp_time++;
	switch (*(sntp_time + 2)) {
	case 'n':
		if (*(sntp_time + 1) == 'a')
			time.month = January;			//一月
		else if (*(sntp_time + 1) == 'u')
			time.month = June;			//六月
		break;
	case 'b':
		time.month = February;
		break;	//二月
	case 'r':
		if (*(sntp_time + 1) == 'a')
			time.month = March;			//三月
		else if (*(sntp_time + 1) == 'p')
			time.month = April;			//四月
		break;
	case 'y':
		time.month = May;
		break;		//五月

	case 'l':
		time.month = July;
		break;		//七月
	case 'g':
		time.month = August;
		break;	//八月
	case 'p':
		time.month = September;
		break;	//九月
	case 't':
		time.month = October;
		break;	//十月
	case 'v':
		time.month = November;
		break;	//十一月
	case 'c':
		time.month = December;
		break;	//十二月
	}

//获取日
	sntp_time = (char *) os_strstr(sntp_time, " ");
	sntp_time++;
	time.day = (*sntp_time - 0x30) * 10 + *(sntp_time + 1) - 0x30;
//获取时
	sntp_time = (char *) os_strstr(sntp_time, " ");
	sntp_time++;
	time.hour = (*sntp_time - 0x30) * 10 + *(sntp_time + 1) - 0x30;
//获取分
	sntp_time = (char *) os_strstr(sntp_time, ":");
	sntp_time++;
	time.minute = (*sntp_time - 0x30) * 10 + *(sntp_time + 1) - 0x30;
//获取秒
	sntp_time = (char *) os_strstr(sntp_time, ":");
	sntp_time++;
	time.second = (*sntp_time - 0x30) * 10 + *(sntp_time + 1) - 0x30;
//获取年
	sntp_time = (char *) os_strstr(sntp_time, " ");
	sntp_time++;
	time.year = (*(sntp_time + 2) - 0x30) * 10 + *(sntp_time + 3) - 0x30;

}

