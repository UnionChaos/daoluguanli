#include "softi2c_c.h"
#include "softi2c_port_c.h"

#ifndef NULL
#define NULL  ((void*)0)
#endif

#define MUL_TASK_CALL           1

#if MUL_TASK_CALL



void    _n_i2c_io_init(void);
void    _i2c_scl_0(uint8_t ch_index);
void    _i2c_scl_1(uint8_t ch_index);
void    _i2c_sda_0(uint8_t ch_index);
void    _i2c_sda_1(uint8_t ch_index);
uint8_t _i2c_scl_is_high(uint8_t ch_index);
uint8_t _i2c_sda_is_high(uint8_t ch_index);
void    _i2c_dly_scl(void);
void    _i2c_dly_ms(uint8_t ms);

static void mutex_create(void)
{
//    OS_ERR err;
//    OSMutexCreate (&iic_bus_mutex,"iic bus mutex",&err);   
}


static void mutex_pend(void)
{
//    OS_ERR err;
//    OSMutexPend (&iic_bus_mutex,0,OS_OPT_PEND_BLOCKING,NULL,&err);
}


static void mutex_post(void)
{
//    OS_ERR err;
//    OSMutexPost (&iic_bus_mutex,OS_OPT_POST_NONE,&err);
}

#else

static void mutex_create(void) {  }
static void mutex_pend(void){ }
static void mutex_post(void){ }

#endif

void softi2c_init(void)
{
	static uint8_t is_called = 0;
	if (is_called)
		return;
	is_called = 1;

	_n_i2c_io_init();
	

		_i2c_scl_1(1);
		_i2c_sda_1(1);

    mutex_create();
}

static void _reset(uint8_t ch_index)
{
	_i2c_sda_1(ch_index);
	for (uint8_t i = 0; i < 10; i++)
	{
		_i2c_scl_0(ch_index);
		_i2c_dly_scl();
		_i2c_scl_1(ch_index);
		_i2c_dly_scl();
	}
}


static void _stop(uint8_t ch_index)
{
	_i2c_sda_0(ch_index);
	_i2c_dly_scl();
	_i2c_scl_1(ch_index);
	_i2c_dly_scl();
	_i2c_sda_1(ch_index);
	_i2c_dly_scl();
}

static uint8_t _start(uint8_t ch_index)
{
	_i2c_sda_1(ch_index);
	_i2c_scl_1(ch_index);
	_i2c_dly_scl();
	
	uint8_t retry = 10;
	while ((!_i2c_sda_is_high(ch_index) || !_i2c_scl_is_high(ch_index)) &&(--retry))
	{
		_stop(ch_index);
		_reset(ch_index);
	}
	if (retry > 0)
	{
		_i2c_sda_0(ch_index);
		_i2c_dly_scl();
		_i2c_scl_0(ch_index);
		_i2c_dly_scl();
		return 1;
	}
	else
	{
		return 0;
	}
}


static uint8_t _tx_byte(uint8_t ch_index, uint8_t b)
{	
	for (uint8_t i = 0; i < 8; i++)
	{
		if (b & 0x80)
			_i2c_sda_1(ch_index);
		else
			_i2c_sda_0(ch_index);
		b <<= 1;
		_i2c_scl_1(ch_index);
		_i2c_dly_scl();
		_i2c_scl_0(ch_index);
		_i2c_dly_scl();
	}
	_i2c_sda_1(ch_index);
	_i2c_dly_scl();
	_i2c_scl_1(ch_index);
	_i2c_dly_scl();
	uint8_t is_ack = !_i2c_sda_is_high(ch_index);
	_i2c_scl_0(ch_index);
	_i2c_dly_scl();

	return is_ack;
}


static void _ack(uint8_t ch_index)
{
	_i2c_sda_0(ch_index);
	_i2c_dly_scl();
	_i2c_scl_1(ch_index);
	_i2c_dly_scl();
	_i2c_scl_0(ch_index);
	_i2c_dly_scl();
}

static void _nack(uint8_t ch_index)
{
	_i2c_sda_1(ch_index);
	_i2c_dly_scl();
	_i2c_scl_1(ch_index);
	_i2c_dly_scl();
	_i2c_scl_0(ch_index);
	_i2c_dly_scl();
}

static uint8_t _rx_byte(uint8_t ch_index)
{
	uint8_t b = 0;
	_i2c_sda_1(ch_index);
	for (uint8_t i = 0; i < 8; i++)
	{
		_i2c_scl_1(ch_index);
		_i2c_dly_scl();
		b <<= 1;
		if (_i2c_sda_is_high(ch_index))
			b |= 0x01;
		_i2c_scl_0(ch_index);
		_i2c_dly_scl();
	}
	return b;
}



// 私有函数(单次传输)
static uint8_t _I2C_Trans(uint8_t ch_index, uint8_t addr, uint8_t* tx_buf, uint16_t tx_len, uint8_t* rx_buf, uint16_t rx_len)
{
	uint8_t is_success = 0;
	
	if ( tx_len > 0 )  
	{
		is_success = _start(ch_index);
		if (!is_success) return SOFTI2C_ERR_LINE;
		is_success = _tx_byte(ch_index, addr);
		if (!is_success) return SOFTI2C_ERR_LINE;        
        for (uint16_t i = 0; i < tx_len; i++)
        {
            is_success = _tx_byte(ch_index, tx_buf[i]);
            if (!is_success && rx_len > 0) return SOFTI2C_ERR_LINE;
        }
    }	
    if (rx_len > 0)
	{
		is_success = _start(ch_index);
		if (!is_success) return SOFTI2C_ERR_LINE;
		is_success = _tx_byte(ch_index, addr | 0x01);
		if (!is_success) return SOFTI2C_ERR_LINE;	
        for (uint16_t i = 0; i < rx_len; i++)
        {
            rx_buf[i] = _rx_byte(ch_index);
            if (i != (rx_len - 1))
                _ack(ch_index);
			else
				_nack(ch_index);
        }
    }
	_stop(ch_index);
	_i2c_dly_ms(10);
	return SOFTI2C_ERR_NONE;
}

uint8_t softi2c_trans(uint8_t ch_index, uint8_t addr, uint8_t* tx_buf, uint16_t tx_len, uint8_t* rx_buf, uint16_t rx_len, uint8_t retry_times)
{
	uint8_t err = 0;
	uint8_t retry = SOFTI2C_RETRY_TIMES_DEFAULT;
	if (retry_times > 0 && retry_times < 100)
		retry = retry_times;
		
    mutex_pend();
	while (retry--)
	{
		err = _I2C_Trans(ch_index, addr, tx_buf, tx_len, rx_buf, rx_len);
		if (err == SOFTI2C_ERR_LINE)
		{
			_i2c_dly_ms(10);
			_reset(ch_index);
		}
		else
		{
			break;
		}
	}
    mutex_post();
	return err;
}


