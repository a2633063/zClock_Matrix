#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__
#include "os_type.h"

#define BCDtoDEC(x) ( ((x)>>4)*10+(x)%0x10  )           //BCD��ת��Ϊʮ���Ʊ�ʾ��ʽ
#define DECtoBCD(x) ( (((x)/10)<<4)+(x)%10  )           //ʮ����ת��ΪBCD���ʾ��ʽ

#define VERSION "v0.0.1"

#define TYPE 10
#define TYPE_NAME "zClock_Matrix"

#define DEVICE_NAME "zClock_Matrix_%02X%02X"
#define MDNS_DEVICE_NAME "zClock_Matrix_%s"

#define USER_CONFIG_VERSION 2

#define SETTING_MQTT_STRING_LENGTH_MAX  64      //���� 4 �ֽڶ��롣
#define NAME_LENGTH 32		//���������ַ�����󳤶�

#define TIME_TASK_NUM 5    //ÿ���������5�鶨ʱ����

typedef struct {
	int8_t hour;      //Сʱ
	int8_t minute;    //����
	uint8_t repeat; //bit7:һ��   bit6-0:����-��һ
	int8_t action;    //����
	int8_t on;    //����
} user_plug_task_config_t;

//�û���������ṹ��
typedef struct {
	char version;
	uint8_t name[NAME_LENGTH];
	uint8_t mqtt_ip[SETTING_MQTT_STRING_LENGTH_MAX];   //mqtt service ip
	uint16_t mqtt_port;        //mqtt service port
	uint8_t mqtt_user[SETTING_MQTT_STRING_LENGTH_MAX];     //mqtt service user
	uint8_t mqtt_password[SETTING_MQTT_STRING_LENGTH_MAX];     //mqtt service user
	uint8_t direction;
	uint8_t on;    //��¼��ǰ����
	user_plug_task_config_t task[TIME_TASK_NUM];

} user_config_t;

extern char rtc_init;
extern user_config_t user_config;

typedef enum {

	DISPLAY_STATE_INIT = 0,
	DISPLAY_STATE_WIFI_DISCONNECTED,
	DISPLAY_STATE_WIFI_CONNECTING,
	DISPLAY_STATE_CONNECTED,
	DISPLAY_STATE_STRING,
	DISPLAY_STATE_TIME,
	DISPLAY_STATE_MAX

} display_state_t;

extern display_state_t display_state;
#endif

