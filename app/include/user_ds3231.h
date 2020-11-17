#ifndef __USER_DS3231_H__
#define __USER_DS3231_H__

#define USER_DS3231_ADDR 0xD0

void user_ds3231_init(void);
bool user_ds3231_write(uint8_t addr, uint8_t pData);
bool user_ds3231_page_write(uint8_t addr, uint8_t *pData, uint16_t len);
uint8 user_ds3231_read(uint8_t addr);
bool user_ds3231_page_read(uint8_t addr, uint8_t *pData, uint16_t len);
#endif
