#include "skrmb_reg_disinp.h"
#include "skrmb_intf.h"
#include "skrmb_reg_coil.h"



skrmb_sta_flg_e skrmb_disinp_read_handle(struct _skrmb_dev_node_t *dev_node)
{
    skrmb_sta_flg_e sta_flg = SKRMB_NO_ERROR;
    uint8_t byte_count = 0;
    uint16_t s_data_index = 0, tmp_crc = 0, data_addr = 0, data_len = 0, bit_count = 0;

    data_addr   = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);
    data_len    = SKRMB_U16_GET(dev_node->rec_buf[4], dev_node->rec_buf[5]);

    dev_node->send_buf[s_data_index++] = dev_node->mb_addr;
    dev_node->send_buf[s_data_index++] = SKRMB_FUNCODE_READ_DISCRETE_INPUTS;
    bit_count = skrmb_coil_search(dev_node->reg_table, SKRMB_REG_TYPE_DISCRETE_INPUT, dev_node->reg_count, data_addr, &dev_node->send_buf[s_data_index + 1], data_len, true);
    byte_count = (bit_count / 8);
    byte_count = (bit_count == 0) ? 0 : (byte_count + (bit_count % 8 != 0 ? 1 : 0));
    dev_node->send_buf[s_data_index++] = byte_count;
    s_data_index += byte_count;

    /* Currently, only valid data will be returned; non-existent data will not be returned, and error codes are not returned for the time being. */
    // if (bit_count < data_len) 
    // {
    //     s_data_index = skrmb_err_response(dev_node, SKRMB_FUNCODE_READ_DISCRETE_INPUTS, SKRMB_ERR_ILLEGAL_DADDR);
    //     sta_flg = SKRMB_REG_INDEX_OVERFLOW;
    // }

    tmp_crc = skrmb_crc(dev_node->send_buf, s_data_index);
    dev_node->send_buf[s_data_index++] = (uint8_t)tmp_crc;
    dev_node->send_buf[s_data_index++] = (uint8_t)(tmp_crc >> 8);

    skrmb_send_data(dev_node, dev_node->send_buf, s_data_index);

    return sta_flg;
}

skrmb_sta_flg_e skrmb_m_disinp_read_handle(struct _skrmb_dev_node_t *dev_node)
{
    uint8_t slave_addr = 0, byte_count = 0;

    slave_addr  = dev_node->rec_buf[0];
    byte_count  = dev_node->rec_buf[2];

    skrmb_m_read_disinp_cb(slave_addr, dev_node->m_wait_para->reg_addr, &dev_node->rec_buf[3], byte_count);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_disinp_request_read(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, uint16_t reg_num)
{
    struct _skrmb_dev_node_t *dev_node = NULL;
    
    dev_node = skrmb_find_dev(dev_id);
    SKRMB_PTR_NULL(dev_node);

    dev_node->m_wait_para->waiting_flg = true;
    dev_node->m_wait_para->smb_addr    = mb_addr;
    dev_node->m_wait_para->funcode     = SKRMB_FUNCODE_READ_DISCRETE_INPUTS;
    dev_node->m_wait_para->reg_addr    = reg_addr;
    dev_node->m_wait_para->reg_num     = reg_num;
    dev_node->m_wait_para->timeout_ms  = 150;
    dev_node->m_wait_para->s_tick_ms   = skrmb_get_curr_tick_ms();

    dev_node->send_port_id             = port_id;

    skrmb_m_request_read(dev_node);

    return SKRMB_NO_ERROR;
}








