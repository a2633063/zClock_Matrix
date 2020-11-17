#ifndef __USER_WIFI_H__
#define __USER_WIFI_H__

typedef enum {

	STATE_WIFI_STAMODE_IDE = 0,
	STATE_WIFI_STAMODE_DISCONNECTED,
	STATE_WIFI_STAMODE_CONNECTING,
	STATE_WIFI_STAMODE_CONNECTED,
	STATE_WIFI_STAMODE_GOT_IP,
	STATE_WIFI_SOFTAPMODE,
	STATE_WIFI_SOFTAPMODE_CONNECTED,


	STATU_MAX

} wifi_state_t;


extern char hwaddr[6];
extern char strMac[16];
extern char strIP[16];


void user_wifi_init(void);
void user_wifi_AP(void);
void user_wifi_set(char *ssid, char *pass);
#endif
