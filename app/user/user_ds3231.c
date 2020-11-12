#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "user_ds3231.h"
#include "driver\i2c_master.h"

void ICACHE_FLASH_ATTR
user_ds3231_init(void) {
	i2c_master_gpio_init();
}

bool ICACHE_FLASH_ATTR
user_ds3231_write(uint8 addr, uint8 pData) {
	uint8 ack, i;
	i2c_master_start();
	i2c_master_writeByte(USER_DS3231_ADDR);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("Addr(0x%x) not ack when writing the device addr\n", USER_DS3231_ADDR);
		i2c_master_stop();
		return false;
	}

	i2c_master_writeByte(addr);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("Addr(0x%x) not ack when writing the register addr\n", USER_DS3231_ADDR);
		i2c_master_stop();
		return false;
	}

	i2c_master_writeByte(pData);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("Addr(0x%x) not ack when writing the register :%x\n", USER_DS3231_ADDR, pData);
		i2c_master_stop();
		return false;
	}

	i2c_master_stop();
}

bool ICACHE_FLASH_ATTR
user_ds3231_page_write(uint8 addr, uint8 *pData, uint16 len) {
	uint8 ack, i;
	i2c_master_start();
	i2c_master_writeByte(USER_DS3231_ADDR);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("Addr(0x%x) not ack when writing the device addr\n", USER_DS3231_ADDR);
		i2c_master_stop();
		return false;
	}

	i2c_master_writeByte(addr);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("Addr(0x%x) not ack when writing the register addr\n", USER_DS3231_ADDR);
		i2c_master_stop();
		return false;
	}

	for (i = 0; i < len; i++) {
		i2c_master_writeByte(*pData);
		ack = i2c_master_checkAck();
		if (!ack) {
			os_printf("Addr(0x%x) not ack when writing the register :%x\n", USER_DS3231_ADDR, *pData);
			i2c_master_stop();
			return false;
		}
		pData++;
	}
	i2c_master_stop();
}
uint8 ICACHE_FLASH_ATTR
user_ds3231_read(uint8 addr) {
	uint8 ack, i;
	i2c_master_start();
	i2c_master_writeByte(USER_DS3231_ADDR + 1);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("Addr(0x%x) not ack when writing the device addr\n", USER_DS3231_ADDR);
		i2c_master_stop();
		return 0;
	}

	i2c_master_writeByte(addr);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("Addr(0x%x) not ack when writing the register addr\n", USER_DS3231_ADDR);
		i2c_master_stop();
		return 0;
	}

	i = i2c_master_readByte();
	i2c_master_send_nack();

	i2c_master_stop();
	return i;
}

bool ICACHE_FLASH_ATTR
user_ds3231_page_read(uint8 addr, uint8 *pData, uint16 len) {
	uint8 ack, i;
	i2c_master_start();
	i2c_master_writeByte(USER_DS3231_ADDR);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("Addr(0x%x) not ack when writing the device addr\n", USER_DS3231_ADDR);
		i2c_master_stop();
		return false;
	}

	i2c_master_writeByte(addr);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("Addr(0x%x) not ack when writing the register addr\n", USER_DS3231_ADDR);
		i2c_master_stop();
		return false;
	}
	i2c_master_stop();
	i2c_master_wait(500);

	i2c_master_start();
	i2c_master_writeByte(USER_DS3231_ADDR + 1);
	ack = i2c_master_checkAck();
	if (!ack) {
		os_printf("Addr(0x%x) not ack when writing the device addr\n", USER_DS3231_ADDR);
		i2c_master_stop();
		return false;
	}

//	i2c_master_writeByte(addr);
//	ack = i2c_master_checkAck();
//	if (!ack) {
//		os_printf("Addr(0x%x) not ack when writing the register addr\n", USER_DS3231_ADDR);
//		i2c_master_stop();
//		return false;
//	}

	for (i = 0; i < len; i++) {
		pData[i] = i2c_master_readByte();
		if (i == (len - 1))
			i2c_master_send_nack();
		else
			i2c_master_send_ack();

	}
	i2c_master_stop();
}
