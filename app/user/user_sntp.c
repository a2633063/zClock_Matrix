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




struct struct_time time;


void time_strtohex(unsigned char *sntp_time)
{
	//��ȡ����
			//��������ΪӢ��Mon,Tues,Wed,Thur,Fri,Sat,Sun �Ƚϵ�2����ĸ(�ܶ���������ͬ�Ƚϵ�һ��)
			switch(sntp_time[1])
			{
				case 'o':time.week=Monday;break;	//��һ
				case 'e':time.week=Wednesday;break;	//����
				case 'h':time.week=Thursday;break;	//����
				case 'r':time.week=Friday;break;	//����
				case 'a':time.week=Saturday;break;	//����
				case 'u':
					if(sntp_time[0]=='S')
						time.week=Sunday;		//����
					else if(sntp_time[0]=='T')
						time.week=Tuesday;		//�ܶ�
					break;
			}

			//��ȡӢ��
			//�Ƚϵ�3����ĸ
			sntp_time=(char *)os_strstr(sntp_time, " ");
			sntp_time++;
			switch(*(sntp_time+2))
			{
				case 'n':
					if(*(sntp_time+1)=='a')
						time.month=January;			//һ��
					else if(*(sntp_time+1)=='u')
						time.month=June;			//����
					break;
				case 'b':time.month=February;break;	//����
				case 'r':
					if(*(sntp_time+1)=='a')
						 time.month=March;			//����
					else if(*(sntp_time+1)=='p')
						 time.month=April;			//����
					break;
				case 'y':time.month=May;break;		//����

				case 'l':time.month=July;break;		//����
				case 'g':time.month=August;break;	//����
				case 'p':time.month=September;break;//����
				case 't':time.month=October;break;	//ʮ��
				case 'v':time.month=November;break;	//ʮһ��
				case 'c':time.month=December;break;	//ʮ����
			}

			//��ȡ��
			sntp_time=(char *)os_strstr(sntp_time, " ");
			sntp_time++;
			time.day=(*sntp_time-0x30)*10+*(sntp_time+1)-0x30;
			//��ȡʱ
			sntp_time=(char *)os_strstr(sntp_time, " ");
			sntp_time++;
			time.hour=(*sntp_time-0x30)*10+*(sntp_time+1)-0x30;
			//��ȡ��
			sntp_time=(char *)os_strstr(sntp_time, ":");
			sntp_time++;
			time.minute=(*sntp_time-0x30)*10+*(sntp_time+1)-0x30;
			//��ȡ��
			sntp_time=(char *)os_strstr(sntp_time, ":");
			sntp_time++;
			time.second=(*sntp_time-0x30)*10+*(sntp_time+1)-0x30;
			//��ȡ��
			sntp_time=(char *)os_strstr(sntp_time, " ");
			sntp_time++;
			time.year=(*(sntp_time+2)-0x30)*10+*(sntp_time+3)-0x30;

}
void ICACHE_FLASH_ATTR
user_sntp_init(void)
{
	sntp_set_timezone (8);//ʱ��:+8
	ip_addr_t *addr = (ip_addr_t *)os_zalloc(sizeof(ip_addr_t));
	ipaddr_aton("210.72.145.44", addr);
	sntp_setserver(0, addr); // set server 0 by IP address
	sntp_setservername(1, "us.pool.ntp.org"); // set server 1 by domain name
	sntp_setservername(2, "ntp.sjtu.edu.cn"); // set server 2 by domain name

	sntp_init();
	os_free(addr);
}

void ICACHE_FLASH_ATTR user_check_sntp_stamp(void)
{
	uint8_t DeviceBuffer[28] = {0};
	uint32 current_stamp;
	current_stamp = sntp_get_current_timestamp();
	if(current_stamp!=0)
	{
		os_sprintf(DeviceBuffer,"%s",sntp_get_real_time(current_stamp));

		time_strtohex(DeviceBuffer);
		os_printf("20%02d/%02d/%02d ��%d %02d:%02d:%02d\n",
				time.year,
				time.month,
				time.day,
				time.week,
				time.hour,
				time.minute,
				time.second);
	}
}

