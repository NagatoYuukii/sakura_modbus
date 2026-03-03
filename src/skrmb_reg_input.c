#include "skrmb_reg_input.h"
#include "skrmb_intf.h"
#include "skrmb_reg_hold.h"

skrmb_sta_flg_e skrmb_input_read_handle(struct _skrmb_dev_node_t *dev_node)
{
    skrmb_sta_flg_e sta_flg = SKRMB_NO_ERROR;
    uint8_t byte_count = 0;
    uint16_t s_data_index = 0, data_addr = 0, data_len = 0;

    data_addr   = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);
    data_len    = SKRMB_U16_GET(dev_node->rec_buf[4], dev_node->rec_buf[5]);

    dev_node->send_buf[s_data_index++] = dev_node->mb_addr;
    dev_node->send_buf[s_data_index++] = SKRMB_FUNCODE_READ_INPUT_REGS;
    byte_count = skrmb_hold_data_handle(dev_node->reg_table, SKRMB_REG_TYPE_INPUT, dev_node->reg_count, data_addr, &dev_node->send_buf[s_data_index + 1], data_len, true);
    dev_node->send_buf[s_data_index++] = byte_count;
    s_data_index += byte_count;

    skrmb_send_data(dev_node, dev_node->send_buf, s_data_index);

    return sta_flg;
}

skrmb_sta_flg_e skrmb_m_input_read_handle(struct _skrmb_dev_node_t *dev_node)
{
    uint8_t slave_addr = 0, byte_count = 0;

    slave_addr  = dev_node->rec_buf[0];
    byte_count  = dev_node->rec_buf[2];

    skrmb_m_read_input_cb(slave_addr, dev_node->m_wait_para->reg_addr, &dev_node->rec_buf[3], byte_count);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_input_request_read(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, uint16_t reg_num)
{
    struct _skrmb_dev_node_t *dev_node = NULL;
    
    dev_node = skrmb_find_dev(dev_id);
    SKRMB_PTR_NULL(dev_node);

    dev_node->m_wait_para->waiting_flg = true;
    dev_node->m_wait_para->smb_addr    = mb_addr;
    dev_node->m_wait_para->funcode     = SKRMB_FUNCODE_READ_INPUT_REGS;
    dev_node->m_wait_para->reg_addr    = reg_addr;
    dev_node->m_wait_para->reg_num     = reg_num;
    dev_node->m_wait_para->timeout_ms  = 150;
    dev_node->m_wait_para->s_tick_ms   = skrmb_get_curr_tick_ms();

    dev_node->send_port_id             = port_id;

    skrmb_m_request_read(dev_node);

    return SKRMB_NO_ERROR;
}


























