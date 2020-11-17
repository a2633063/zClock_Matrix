#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "user_max7219.h"
#include "driver\spi.h"

uint8_t display[4][8];
uint8_t brightness = 1;
uint8_t brightness_on = 1;
extern unsigned char ASCII_Font_5x8[96][8];
extern unsigned char NUM_Font_4x8[10][8];
extern unsigned char NUM_Font_3x6[10][6];

LOCAL os_timer_t timer_max7219;

static void ICACHE_FLASH_ATTR
max7219_set_reg(uint8_t reg, uint8_t dat) {
	uint8_t i;
	GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 0);
	for (i = 0; i < 4; i++) {
		spi_mast_byte_write(HSPI, reg);
		spi_mast_byte_write(HSPI, dat);
	}
	GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 1);
}

void user_max7219_timer_func(void *arg) {
	uint8_t i, j;
	for (i = 0; i < 8; i++) {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 0);
		for (j = 0; j < 4; j++) {
			spi_mast_byte_write(HSPI, i + 1);
			if (user_config.direction == 1) {
				SET_PERI_REG_MASK(SPI_CTRL(HSPI), SPI_WR_BIT_ORDER);
				SET_PERI_REG_MASK(SPI_CTRL(HSPI), SPI_RD_BIT_ORDER);
				spi_mast_byte_write(HSPI, display[j][7 - i]);
			} else
				spi_mast_byte_write(HSPI, display[3 - j][i]);
			if (user_config.direction == 1) {
				CLEAR_PERI_REG_MASK(SPI_CTRL(HSPI), SPI_WR_BIT_ORDER);
				CLEAR_PERI_REG_MASK(SPI_CTRL(HSPI), SPI_RD_BIT_ORDER);
			}
		}
		GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 1);
	}

	user_max7219_set_on(brightness_on);
	user_max7219_set_brightness(brightness-1);
}

void ICACHE_FLASH_ATTR
user_max7219_init(void) {
	uint8_t max7219_init_dat[7][2] = {
	//max7219 init dat
			{ 0x0C, 0x00 },    // display off
			{ 0x00, 0xFF },    // no LEDtest
			{ 0x09, 0x00 },    // BCD off
			{ 0x0F, 0x00 },    // normal operation
			{ 0x0B, 0x07 },    // start display
			{ 0x0A, 0x04 },    // brightness
			{ 0x0C, 0x01 }    // display on
	};

	uint8_t i, j;
	spi_master_init(HSPI);
//    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2);//configure io to spi mode
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2);    //configure io to spi mode
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2);    //configure io to spi mode
//    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2);//configure io to spi mode

	CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX, BIT9);
	PIN_FUNC_SELECT(MAX7219_CS_IO_MUX, MAX7219_CS_IO_FUNC);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 1);
	os_delay_us(100);

	for (i = 0; i < 7; i++) {
		max7219_set_reg(max7219_init_dat[i][0], max7219_init_dat[i][1]);
	}
	user_max7219_clear(0);
	user_max7219_timer_func(NULL);
	//	os_timer_disarm(&timer_tm1628);
	os_timer_setfn(&timer_max7219, (os_timer_func_t *) user_max7219_timer_func, NULL);
	os_timer_arm(&timer_max7219, 50, 1);	//每150ms刷新一次显示

}

void ICACHE_FLASH_ATTR
user_max7219_clear(uint8_t dat) {
	uint8_t i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 8; j++) {
			display[i][j] = dat;
		}
	}
//	for (i = 0; i < 8; i++) {
//		max7219_set_reg(i + 1, dat);
//	}
}

void ICACHE_FLASH_ATTR
user_max7219_set_brightness(uint8_t bri) {
	uint8_t i;
	if (bri > 15)
		bri = 15;
	max7219_set_reg(0x0a, bri);
}
void ICACHE_FLASH_ATTR
user_max7219_set_on(uint8_t on) {
	uint8_t i;
	if (on > 1)
		on = 1;
	max7219_set_reg(0x0c, on);
}

#define FONT_WIDTH_6

void ICACHE_FLASH_ATTR
user_max7219_dis_char(char ch, int8_t x, int8_t y) {
	int8_t i = 0, j, k;
	int8_t x_d, x_r;	//x_d  x_r

	x_d = x / 8;
	x_r = x % 8;

	//处理x y为-1时情况
	if (x < -5)
		return;
	else if (x < 0) {
		x_d = -1;
		x_r = 8 - (-x) % 8;
	}

	if (y < -7)
		return;
	else if (y < 0)
		i = -y;

	ch -= ' ';
	for (i; i < 8 && y + i < 8; i++) {
#ifdef FONT_WIDTH_6
		if (x_d >= 0 && x_d < 4)
			display[x_d][y + i] = display[x_d][y + i] & (~(0xfc >> x_r)) | (ASCII_Font_5x8[ch][i] >> x_r);
		if (x_d + 1 >= 0 && x_d + 1 < 4)
			display[x_d + 1][y + i] = display[x_d + 1][y + i] & (~(0xfc << (8 - x_r))) | (ASCII_Font_5x8[ch][i] << (8 - x_r));
#else
		display[x_d][y + i] = display[x_d][y + i] & (~(0xf8 >> x_r)) | (ASCII_Font_5x8[ch][i] >> x_r);
		if (x_d + 1 < 4)
		display[x_d + 1][y + i] = display[x_d + 1][y + i] & (~(0xf8 << (8 - x_r))) | (ASCII_Font_5x8[ch][i] << (8 - x_r));
#endif
	}
}

void ICACHE_FLASH_ATTR
user_max7219_dis_str(char * ch, int8_t x, int8_t y) {
	while (*ch != 0) {
		user_max7219_dis_char(*ch, x, y);
		x += 6;
		ch++;
	}
}



void ICACHE_FLASH_ATTR
user_max7219_dis_num(char ch, int8_t x, int8_t y) {
	int8_t i = 0, j, k;
	int8_t x_d, x_r;	//x_d  x_r

	x_d = x / 8;
	x_r = x % 8;

	//处理x y为-1时情况
	if (x < -5)
		return;
	else if (x < 0) {
		x_d = -1;
		x_r = 8 - (-x) % 8;
	}

	if (y < -7)
		return;
	else if (y < 0)
		i = -y;

	ch %= 10;
	for (i; i < 8 && y + i < 8; i++) {
		if (x_d >= 0 && x_d < 4)
			display[x_d][y + i] = display[x_d][y + i] & (~(0xf8 >> x_r)) | (NUM_Font_4x8[ch][i] >> x_r);
		if (x_d + 1 >= 0 && x_d + 1 < 4)
			display[x_d + 1][y + i] = display[x_d + 1][y + i] & (~(0xf8 << (8 - x_r))) | (NUM_Font_4x8[ch][i] << (8 - x_r));
	}
}



void ICACHE_FLASH_ATTR
user_max7219_dis_num_small(char ch, int8_t x, int8_t y) {
	int8_t i = 0, j, k;
	int8_t x_d, x_r;	//x_d  x_r

	x_d = x / 8;
	x_r = x % 8;

	//处理x y为-1时情况
	if (x < -4)
		return;
	else if (x < 0) {
		x_d = -1;
		x_r = 8 - (-x) % 8;
	}

	if (y < -5)
		return;
	else if (y < 0)
		i = -y;

	ch %= 10;
	for (i; i < 6 && y + i < 8; i++) {
		if (x_d >= 0 && x_d < 4)
			display[x_d][y + i] = display[x_d][y + i] & (~(0xf0 >> x_r)) | (NUM_Font_3x6[ch][i] >> x_r);
		if (x_d + 1 >= 0 && x_d + 1 < 4)
			display[x_d + 1][y + i] = display[x_d + 1][y + i] & (~(0xf0 << (8 - x_r))) | (NUM_Font_3x6[ch][i] << (8 - x_r));
	}
}
