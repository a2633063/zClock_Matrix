#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "user_max7219.h"
#include "driver\spi.h"
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

	unsigned short i, j;
	spi_master_init(HSPI);
//    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2);//configure io to spi mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2);//configure io to spi mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2);//configure io to spi mode
//    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2);//configure io to spi mode

    //CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX, BIT9);
	PIN_FUNC_SELECT(MAX7219_CS_IO_MUX, MAX7219_CS_IO_FUNC);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 1);
	os_delay_us(2000);

//	GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 0);
//	os_delay_us(100);
//	spi_mast_byte_write(HSPI, 0X01);
//	spi_mast_byte_write(HSPI, 0X02);
//	spi_mast_byte_write(HSPI, 0X03);
//	os_delay_us(100);
//	GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 1);
//	os_delay_us(100);
//	spi_mast_byte_write(HSPI, 0X04);
//	spi_mast_byte_write(HSPI, 0X05);


	for (i = 0; i < 7; i++) {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 0);
		os_delay_us(10);
		for (j = 0; j < 4; j++) {
			spi_mast_byte_write(HSPI, max7219_init_dat[i][0]);     //register
			spi_mast_byte_write(HSPI, max7219_init_dat[i][1]);     //value
		}
		os_delay_us(10);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 1);
	}
	user_max7219_clear();
}

void ICACHE_FLASH_ATTR
user_max7219_clear() {
	unsigned short i, j;
	for (i = 0; i < 8; i++) {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 0);
		os_delay_us(10);
		for (j = 4; j > 0; j--) {
			spi_mast_byte_write(HSPI, i + 1);     //register
			spi_mast_byte_write(HSPI, i*8+j);     //value
		}os_delay_us(10);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(MAX7219_CS_IO_NUM), 1);
	}
}
