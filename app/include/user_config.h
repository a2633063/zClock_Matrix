#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__
#include "os_type.h"

#define BCDtoDEC(x) ( ((x)>>4)*10+(x)%0x10  )           //BCD码转换为十进制表示方式
#define DECtoBCD(x) ( (((x)/10)<<4)+(x)%10  )           //十进制转换为BCD码表示方式

#define VERSION "v0.0.1"

#define TYPE 10
#define TYPE_NAME "zClock_Matrix"

#define DEVICE_NAME "zClock_Matrix_%02X%02X"
#define MDNS_DEVICE_NAME "zClock_Matrix_%s"

#define USER_CONFIG_VERSION 2

#define SETTING_MQTT_STRING_LENGTH_MAX  64      //必须 4 字节对齐。
#define NAME_LENGTH 32		//插座名称字符串最大长度

#define TIME_TASK_NUM 5    //每个插座最多5组定时任务

typedef struct {
	int8_t hour;      //小时
	int8_t minute;    //分钟
	uint8_t repeat; //bit7:一次   bit6-0:周日-周一
	int8_t action;    //动作
	int8_t on;    //开关
} user_plug_task_config_t;

//用户保存参数结构体
typedef struct {
	char version;
	uint8_t name[NAME_LENGTH];
	uint8_t mqtt_ip[SETTING_MQTT_STRING_LENGTH_MAX];   //mqtt service ip
	uint16_t mqtt_port;        //mqtt service port
	uint8_t mqtt_user[SETTING_MQTT_STRING_LENGTH_MAX];     //mqtt service user
	uint8_t mqtt_password[SETTING_MQTT_STRING_LENGTH_MAX];     //mqtt service user
	uint8_t direction;
	uint8_t on;    //记录当前开关
	user_plug_task_config_t task[TIME_TASK_NUM];

} user_config_t;

extern char rtc_init;
extern user_config_t user_config;

typedef enum {

	DISPLAY_STATE_INIT = 0, DISPLAY_STATE_WIFI_DISCONNECTED,	//断开
	DISPLAY_STATE_WIFI_CONNECTING,		//正在连接
	DISPLAY_STATE_AP_WEB,				//ap热点配网模式 显示AP WEB
	DISPLAY_STATE_CONNECTED,			//已连接,已获取到ip
	DISPLAY_STATE_OK,					//显示OK!!
	DISPLAY_STATE_ERR,					//显示Err!
	DISPLAY_STATE_CON,					//显示CON!
	DISPLAY_STATE_AP_CONFIRM,			//显示AP? 按下按键后进入配网模式

	DISPLAY_STATE_STRING,				//显示滚动字符串
	DISPLAY_STATE_TIME,					//正常显示时间
	DISPLAY_STATE_MAX

} display_state_t;

extern display_state_t display_state;
#endif

