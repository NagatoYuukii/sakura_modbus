#include "skrmb_intf.h"

void *skrmb_malloc(uint32_t size)
{
    return malloc(size);
}


void skrmb_free(void *ptr)
{
    if (ptr == NULL) return;
    free(ptr);
}

uint16_t skrmb_crc(uint8_t *data, uint16_t len)
{
    return skrmb_default_crc(data, len);
}

uint32_t skrmb_get_curr_tick_ms(void)
{
	return 0;
}

void skrmb_delay_ms(uint32_t ms)
{
    
}

void skrmb_m_read_coil_cb(uint8_t mb_addr, uint16_t reg_addr, uint8_t *coil_data, uint16_t byte_len)
{

}

void skrmb_m_read_disinp_cb(uint8_t mb_addr, uint16_t reg_addr, uint8_t *disinp_data, uint16_t byte_len)
{

}

void skrmb_m_write_coil_cb(uint8_t mb_addr, uint16_t reg_addr, uint16_t coil_num)
{

}

void skrmb_m_read_hold_cb(uint8_t mb_addr, uint16_t reg_addr, uint8_t *hold_data, uint16_t byte_len)
{

}

void skrmb_m_read_input_cb(uint8_t mb_addr, uint16_t reg_addr, uint8_t *input_data, uint16_t byte_len)
{
    
}

void skrmb_m_write_hold_cb(uint8_t mb_addr, uint16_t reg_addr, uint16_t write_reg_num)
{

}

void skrmb_m_wait_timeout_cb(uint8_t mb_addr)
{
    
}

