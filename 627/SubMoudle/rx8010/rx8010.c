#include "softi2c_c.h"
#include "rx8010.h"

#ifndef NULL
#define NULL            	(void*)(0)
#endif

// 芯片 : RX-8010SJ
#define SLAVE_ADDR     		0x64
#define RX8010_I2C_INDEX	1


static uint32_t ErrCnt = 0;


#define BCD_TO_BYTE(val)     ((val) - 6 * ((val) >> 4))
#define BYTE_TO_BCD(val)     ((val) + 6 * ((val) / 10))

static void rx8010_check(void)
{
    uint8_t tx_buf[1] = {0x1E};
	uint8_t rx_buf[1];
	uint8_t err = softi2c_trans(RX8010_I2C_INDEX, SLAVE_ADDR, tx_buf, sizeof(tx_buf), rx_buf, sizeof(rx_buf),3);
    if (err == SOFTI2C_ERR_NONE)
    {
        if(rx_buf[0]& (1 << 1))  //VLF
        {
            uint8_t buf[1+1] = {0x1E};
            buf[1] = rx_buf[0]&(~(1<<1));
            softi2c_trans(RX8010_I2C_INDEX, SLAVE_ADDR, buf, sizeof(buf), (void *)0, 0 , 3);
        }
    }
}

static void rx8010_default(void)
{
    uint8_t tx_buf[2] = {0x17 , 0xD8};
  uint8_t err = softi2c_trans(RX8010_I2C_INDEX, SLAVE_ADDR, tx_buf, sizeof(tx_buf), NULL, 0,3);

	
        uint8_t tx_buf1[1] = {0x1F};

	uint8_t rx_buf1[1];
	err = softi2c_trans(RX8010_I2C_INDEX, SLAVE_ADDR, tx_buf1, sizeof(tx_buf1), rx_buf1, sizeof(rx_buf1),3);
    if (err == SOFTI2C_ERR_NONE)
    {
		uint8_t buf[1+1] = {0x1F};
		buf[1] = rx_buf1[0] & (~(1<<7)) & (~(3 << 0)) & (~(1<<4)) & (~(1<<3)) &( ~(1 << 6));
		softi2c_trans(RX8010_I2C_INDEX, SLAVE_ADDR, buf, sizeof(buf), (void *)0, 0 , 3);       
    }


	tx_buf[0] = 0x30;
	tx_buf[1] = 0x0;
	err = softi2c_trans(RX8010_I2C_INDEX, SLAVE_ADDR, tx_buf, sizeof(tx_buf), NULL, 0,3);
    if (err != SOFTI2C_ERR_NONE)
	
	tx_buf[0] = 0x31;
	tx_buf[1] = 0x08;
	err = softi2c_trans(RX8010_I2C_INDEX, SLAVE_ADDR, tx_buf, sizeof(tx_buf), NULL, 0,3);

        uint8_t tx_buf2[1] = {0x32};
	uint8_t rx_buf2[1];
        //tx_buf[0] = 0x32;
	//tx_buf[1] = 0x00;
	err = softi2c_trans(RX8010_I2C_INDEX, SLAVE_ADDR, tx_buf2, sizeof(tx_buf2), rx_buf2, sizeof(rx_buf2),3);
    if (err == SOFTI2C_ERR_NONE)
    {
            uint8_t buf[1+1] = {0x32};
            buf[1] = rx_buf2[0] & (~(31<<3));
            softi2c_trans(RX8010_I2C_INDEX, SLAVE_ADDR, buf, sizeof(buf), (void *)0, 0 , 3);       
    }

}

void rx8010_Init(void)
{
	softi2c_init();
    rx8010_check();
	rx8010_default();
}



uint8_t rx_buf[7] = {0};
void rx8010_get_time(rx8010_time_t * time)
{

	uint8_t tx_buf[1] = {0x10};
	
	uint8_t err = softi2c_trans(RX8010_I2C_INDEX, SLAVE_ADDR, tx_buf, sizeof(tx_buf), rx_buf, sizeof(rx_buf),1);
	if (err == SOFTI2C_ERR_NONE)
	{
		rx_buf[0] &= 0x7F;                 //秒
		rx_buf[1] &= 0x7F;                 //分
		rx_buf[2] &= 0x3F;                 //时
		//rx_buf[3] &= 0x3F;                 //星期
        rx_buf[4] &= 0x3F;                 //日
		rx_buf[5] &= 0x1F;                 //月

		time->sec = BCD_TO_BYTE(rx_buf[0]);
		time->min = BCD_TO_BYTE(rx_buf[1]);
		time->hour = BCD_TO_BYTE(rx_buf[2]);
		time->week = BCD_TO_BYTE(rx_buf[3]);
		time->day = BCD_TO_BYTE(rx_buf[4]);
		time->month = BCD_TO_BYTE(rx_buf[5]);
		time->year = BCD_TO_BYTE(rx_buf[6])+ 2000;

	}
	else
	{
		ErrCnt++;
	}	
}

void rx8010_set_time(rx8010_time_t * time)
{
	uint8_t tx_buf[1 + 7] = {0x10};
	tx_buf[1] = BYTE_TO_BCD(time->sec);
	tx_buf[2] = BYTE_TO_BCD(time->min);
	tx_buf[3] = BYTE_TO_BCD(time->hour);
	tx_buf[4] = BYTE_TO_BCD(time->week);
	tx_buf[5] = BYTE_TO_BCD(time->day);
	tx_buf[6] = BYTE_TO_BCD(time->month);
	tx_buf[7] = BYTE_TO_BCD(time->year % 100);
	uint8_t err = softi2c_trans(RX8010_I2C_INDEX, SLAVE_ADDR, tx_buf, sizeof(tx_buf), (void *)0, 0,3);
	if (err == SOFTI2C_ERR_NONE)
	{

	}
	else
	{	
		ErrCnt++;
	}
}


uint8_t rx8010_IsErr(void)
{
	if(ErrCnt)
		return 1;
	else
		return 0;
}