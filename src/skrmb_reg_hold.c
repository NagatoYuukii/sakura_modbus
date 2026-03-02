#include "skrmb_reg_hold.h"
#include "skrmb_intf.h"

uint8_t skrmb_hold_data_handle(struct _skrmb_dev_reg_t *reg_table, skrmb_reg_type_e reg_type, uint16_t reg_count, uint16_t addr, uint8_t *buf, uint16_t len, bool is_read)
{
    uint8_t byte_count = 0;        // 已填充的buf字节索引
    uint16_t *data_p = NULL;
    uint32_t i = 0;

    for (i = 0; i < reg_count; i++)
    {
        if (reg_table[i].reg_type != reg_type) {
            continue;
        }

        data_p = (uint16_t *)reg_table[i].reg_data;
        
        bool single_byte = false;
        uint16_t reg_len = reg_table[i].reg_len;
        uint16_t reg_start = reg_table[i].start_addr;
        /* 寄存器长度传入的是 uint8_t 的长度，而标准modbus是uint16_t */
        if (reg_len % 2 != 0) {
            reg_len++;
            single_byte = true;
        }
        uint16_t reg_end = reg_table[i].start_addr + (reg_len / 2) - 1;

        if (addr < reg_start || addr > reg_end) {
            continue;
        }

        uint16_t copy_count = (reg_end - addr + 1) * 2;
        if (reg_end - addr >= len) {
            // 超出了范围了
            copy_count = len * 2;
            single_byte = false;
        }

        if (is_read) {
            memcpy(&buf[byte_count], data_p, single_byte ? (copy_count - 1) : copy_count);
            skrmb_reverse_two_bytes(&buf[byte_count], copy_count);
        } else {
            skrmb_reverse_two_bytes(&buf[byte_count], copy_count);
            memcpy(data_p, &buf[byte_count], single_byte ? (copy_count - 1) : copy_count);
        }
        byte_count += copy_count;
        addr += (copy_count / 2);
        len -= (copy_count / 2);
    }
    return byte_count;
}

skrmb_sta_flg_e skrmb_hold_read_handle(struct _skrmb_dev_node_t *dev_node)
{
    skrmb_sta_flg_e sta_flg = SKRMB_NO_ERROR;
    uint8_t byte_count = 0;
    uint16_t s_data_index = 0, tmp_crc = 0, data_addr = 0, data_len = 0;

    data_addr   = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);
    data_len    = SKRMB_U16_GET(dev_node->rec_buf[4], dev_node->rec_buf[5]);

    dev_node->send_buf[s_data_index++] = dev_node->mb_addr;
    dev_node->send_buf[s_data_index++] = SKRMB_FUNCODE_READ_HOLDING_REGS;
    byte_count = skrmb_hold_data_handle(dev_node->reg_table, SKRMB_REG_TYPE_HOLDING, dev_node->reg_count, data_addr, &dev_node->send_buf[s_data_index + 1], data_len, true);
    dev_node->send_buf[s_data_index++] = byte_count;
    s_data_index += byte_count;

    tmp_crc = skrmb_crc(dev_node->send_buf, s_data_index);
    dev_node->send_buf[s_data_index++] = (uint8_t)tmp_crc;
    dev_node->send_buf[s_data_index++] = (uint8_t)(tmp_crc >> 8);

    skrmb_send_data(dev_node, dev_node->send_buf, s_data_index);

    return sta_flg;
}

skrmb_sta_flg_e skrmb_hold_write_single_handle(struct _skrmb_dev_node_t *dev_node)
{
    skrmb_sta_flg_e sta_flg = SKRMB_NO_ERROR;
    uint16_t s_data_index = 0, tmp_crc = 0, data_addr = 0;

    data_addr   = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);

    dev_node->send_buf[s_data_index++] = dev_node->mb_addr;
    dev_node->send_buf[s_data_index++] = SKRMB_FUNCODE_WRITE_SINGLE_HOLDING_REG;
    dev_node->send_buf[s_data_index++] = (data_addr >> 8);
    dev_node->send_buf[s_data_index++] = (uint8_t)data_addr;
    dev_node->send_buf[s_data_index++] = dev_node->rec_buf[4];
    dev_node->send_buf[s_data_index++] = dev_node->rec_buf[5];

    /* note: skrmb_hold_data_handle 可能会将数据反转所以这条指令需要在 dev_node->send_buf[s_data_index++] = dev_node->rec_buf[4]; 后面  */
    skrmb_hold_data_handle(dev_node->reg_table, SKRMB_REG_TYPE_HOLDING, dev_node->reg_count, data_addr, (uint8_t *)&dev_node->rec_buf[4], 1, false);

    tmp_crc = skrmb_crc(dev_node->send_buf, s_data_index);
    dev_node->send_buf[s_data_index++] = (uint8_t)tmp_crc;
    dev_node->send_buf[s_data_index++] = (uint8_t)(tmp_crc >> 8);

    skrmb_send_data(dev_node, dev_node->send_buf, s_data_index);

    return sta_flg;
}

skrmb_sta_flg_e skrmb_hold_write_multiple_handle(struct _skrmb_dev_node_t *dev_node)
{
    skrmb_sta_flg_e sta_flg = SKRMB_NO_ERROR;
    uint8_t byte_count = 0, reg_w_count = 0;
    uint16_t s_data_index = 0, tmp_crc = 0, data_addr = 0, data_len = 0;

    data_addr   = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);
    data_len    = SKRMB_U16_GET(dev_node->rec_buf[4], dev_node->rec_buf[5]);
    byte_count  = dev_node->rec_buf[6];

    byte_count = skrmb_hold_data_handle(dev_node->reg_table, SKRMB_REG_TYPE_HOLDING, dev_node->reg_count, data_addr, (uint8_t *)&dev_node->rec_buf[7], data_len, false);
    reg_w_count = (byte_count / 2);

    dev_node->send_buf[s_data_index++] = dev_node->mb_addr;
    dev_node->send_buf[s_data_index++] = SKRMB_FUNCODE_WRITE_MULTIPLE_HOLDING_REGS;
    dev_node->send_buf[s_data_index++] = (data_addr >> 8);
    dev_node->send_buf[s_data_index++] = (uint8_t)data_addr;
    dev_node->send_buf[s_data_index++] = (reg_w_count >> 8);
    dev_node->send_buf[s_data_index++] = (uint8_t)reg_w_count;

    tmp_crc = skrmb_crc(dev_node->send_buf, s_data_index);
    dev_node->send_buf[s_data_index++] = (uint8_t)tmp_crc;
    dev_node->send_buf[s_data_index++] = (uint8_t)(tmp_crc >> 8);

    skrmb_send_data(dev_node, dev_node->send_buf, s_data_index);

    return sta_flg;
}

skrmb_sta_flg_e skrmb_m_hold_read_handle(struct _skrmb_dev_node_t *dev_node)
{
    uint8_t slave_addr = 0, byte_count = 0;

    slave_addr  = dev_node->rec_buf[0];
    byte_count  = dev_node->rec_buf[2];

    skrmb_m_read_hold_cb(slave_addr, dev_node->m_wait_para->reg_addr, &dev_node->rec_buf[3], byte_count);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_hold_write_single_handle(struct _skrmb_dev_node_t *dev_node)
{
    uint8_t slave_addr = 0, write_reg_num = 0;
    uint16_t reg_addr = 0;

    slave_addr  = dev_node->rec_buf[0];
    reg_addr    = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);
    write_reg_num = 1;

    skrmb_m_write_hold_cb(slave_addr, reg_addr, write_reg_num);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_hold_write_multiple_handle(struct _skrmb_dev_node_t *dev_node)
{
    uint8_t slave_addr = 0, write_reg_num = 0;
    uint16_t reg_addr = 0;

    slave_addr  = dev_node->rec_buf[0];
    reg_addr    = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);
    write_reg_num = SKRMB_U16_GET(dev_node->rec_buf[4], dev_node->rec_buf[5]);

    skrmb_m_write_hold_cb(slave_addr, reg_addr, write_reg_num);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_hold_request_read(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, uint16_t reg_num)
{
    struct _skrmb_dev_node_t *dev_node = NULL;
    
    dev_node = skrmb_find_dev(dev_id);
    SKRMB_PTR_NULL(dev_node);

    dev_node->m_wait_para->waiting_flg = true;
    dev_node->m_wait_para->smb_addr    = mb_addr;
    dev_node->m_wait_para->funcode     = SKRMB_FUNCODE_READ_HOLDING_REGS;
    dev_node->m_wait_para->reg_addr    = reg_addr;
    dev_node->m_wait_para->reg_num     = reg_num;
    dev_node->m_wait_para->timeout_ms  = 150;
    dev_node->m_wait_para->s_tick_ms   = skrmb_get_curr_tick_ms();

    dev_node->send_port_id             = port_id;

    skrmb_m_request_read(dev_node);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_hold_request_single_write(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, uint16_t value)
{
    struct _skrmb_dev_node_t *dev_node = NULL;

    dev_node = skrmb_find_dev(dev_id);
    SKRMB_PTR_NULL(dev_node);

    dev_node->m_wait_para->waiting_flg = true;
    dev_node->m_wait_para->smb_addr    = mb_addr;
    dev_node->m_wait_para->funcode     = SKRMB_FUNCODE_WRITE_SINGLE_HOLDING_REG;
    dev_node->m_wait_para->reg_addr    = reg_addr;
    dev_node->m_wait_para->reg_num     = 1;
    dev_node->m_wait_para->timeout_ms  = 150;
    dev_node->m_wait_para->s_tick_ms   = skrmb_get_curr_tick_ms();
    dev_node->send_port_id             = port_id;

    skrmb_m_request_write(dev_node, &value, sizeof(value));

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_hold_request_multiple_write(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, uint8_t *data, uint16_t data_len)
{
    struct _skrmb_dev_node_t *dev_node = NULL;

    dev_node = skrmb_find_dev(dev_id);
    SKRMB_PTR_NULL(dev_node);

    dev_node->m_wait_para->waiting_flg = true;
    dev_node->m_wait_para->smb_addr    = mb_addr;
    dev_node->m_wait_para->funcode     = SKRMB_FUNCODE_WRITE_MULTIPLE_HOLDING_REGS;
    dev_node->m_wait_para->reg_addr    = reg_addr;
    dev_node->m_wait_para->reg_num     = data_len / 2;
    dev_node->m_wait_para->timeout_ms  = 150;
    dev_node->m_wait_para->s_tick_ms   = skrmb_get_curr_tick_ms();
    dev_node->send_port_id             = port_id;

    if (data_len % 2 != 0) {
        skrmb_debug("Modbus requires data to be in units of two bytes !\r\n");
    };

    skrmb_m_request_write(dev_node, data, (data_len / 2) * 2);

    return SKRMB_NO_ERROR;
}













