#include "skrmb_reg_coil.h"
#include "skrmb_intf.h"


uint16_t skrmb_coil_search(struct _skrmb_dev_reg_t *reg_table, skrmb_reg_type_e reg_type, uint16_t reg_count, uint16_t addr, uint8_t *buf, uint16_t len, bool is_read)
{
    uint32_t i = 0;
    uint16_t bit_count = 0;        // 已读取的线圈位数（0~7）
    uint8_t byte_count = 0;        // 已填充的buf字节索引
    uint16_t data_p_bit_idx = 0;   // 当前data_p中的位偏移（0~7）
    uint16_t data_p_byte_idx = 0;  // 当前data_p中的字节偏移
    uint8_t *data_p = NULL;

    // skrmb_debug("reg_count = %d addr = %d len = %d\r\n", reg_count, addr, len);

    for (i = 0; i < reg_count; i++) 
    {
        if (reg_table[i].reg_type != reg_type) {
            continue;
        }

        data_p = (uint8_t *)reg_table[i].reg_data;
    
        uint16_t reg_start = reg_table[i].start_addr;
        uint16_t reg_end = reg_table[i].start_addr + reg_table[i].reg_len - 1;

        if (addr > reg_end) {
            continue;
        }

        while (bit_count < len) 
        {
            // 当前addr超出寄存器范围
            if (addr < reg_start || addr > reg_end) {
                break;
            }
            // 线圈地址偏移 = 当前addr - 寄存器起始地址
            uint16_t coil_offset = addr - reg_start;
            data_p_byte_idx = coil_offset / 8;    // 线圈在data_p中的字节索引
            data_p_bit_idx = coil_offset % 8;     // 线圈在字节中的位索引（0=bit0，7=bit7）

            if (is_read) {
                // 提取当前线圈的状态（1=ON，0=OFF），写入buf对应位
                if (data_p[data_p_byte_idx] & (1 << data_p_bit_idx)) {
                    // buf的对应位置1（buf_byte_count=bit_count/8，buf_bit_idx=bit_count%8）
                    buf[byte_count] |= (1 << (bit_count % 8));
                }
            } else {
                // 写逻辑：根据buf对应位，设置data_p的对应位（1置1，0清0）
                uint8_t buf_bit_idx = bit_count % 8; // buf中当前位的索引
                if (buf[byte_count] & (1 << buf_bit_idx)) {
                    data_p[data_p_byte_idx] |= (1 << data_p_bit_idx);
                } else {
                    data_p[data_p_byte_idx] &= ~(1 << data_p_bit_idx);
                }
            }

            // 已读取位数+1
            bit_count++;
            // 若当前字节的8位已填满，切换到下一个字节
            if (bit_count % 8 == 0) {
                byte_count++;
            }
            // 目标addr递增（读取下一个线圈）
            addr++;
        }
    }
    // 返回实际填充的字节数（若最后一个字节未填满，也计入）
    // skrmb_debug("bit_count = %d, byte_count = %d\r\n", bit_count, byte_count);
    // return (bit_count == 0) ? 0 : (byte_count + (bit_count % 8 != 0 ? 1 : 0));
    return bit_count;
}


skrmb_sta_flg_e skrmb_coil_read_handle(struct _skrmb_dev_node_t *dev_node)
{
    uint8_t byte_count = 0;
    uint16_t s_data_index = 0, data_addr = 0, data_len = 0, bit_count = 0;

    data_addr   = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);
    data_len    = SKRMB_U16_GET(dev_node->rec_buf[4], dev_node->rec_buf[5]);

    dev_node->send_buf[s_data_index++] = dev_node->mb_addr;
    dev_node->send_buf[s_data_index++] = SKRMB_FUNCODE_READ_COILS;
    bit_count = skrmb_coil_search(dev_node->reg_table, SKRMB_REG_TYPE_COIL, dev_node->reg_count, data_addr, &dev_node->send_buf[s_data_index + 1], data_len, true);
    byte_count = (bit_count / 8);
    byte_count = (bit_count == 0) ? 0 : (byte_count + (bit_count % 8 != 0 ? 1 : 0));
    dev_node->send_buf[s_data_index++] = byte_count;
    s_data_index += byte_count;

    skrmb_send_data(dev_node, dev_node->send_buf, s_data_index);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_coil_write_single_handle(struct _skrmb_dev_node_t *dev_node)
{
    uint16_t data_addr = 0, s_data_index = 0;
    bool on_off_flg = false;

    data_addr   = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);

    if (dev_node->rec_buf[4] == 0xFF && dev_node->rec_buf[5] == 0x00) {
        on_off_flg = true;
    }

    skrmb_coil_search(dev_node->reg_table, SKRMB_REG_TYPE_COIL, dev_node->reg_count, data_addr, (uint8_t *)&on_off_flg, 1, false);

    dev_node->send_buf[s_data_index++] = dev_node->mb_addr;
    dev_node->send_buf[s_data_index++] = SKRMB_FUNCODE_WRITE_SINGLE_COIL;
    dev_node->send_buf[s_data_index++] = (uint8_t)(data_addr >> 8);
    dev_node->send_buf[s_data_index++] = (uint8_t)(data_addr);
    dev_node->send_buf[s_data_index++] = (uint8_t)(on_off_flg ? 0xFF : 0x00);
    dev_node->send_buf[s_data_index++] = 0x00;

    skrmb_send_data(dev_node, dev_node->send_buf, s_data_index);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_coil_write_multiple_handle(struct _skrmb_dev_node_t *dev_node)
{
    uint8_t  w_byte_count = 0;
    uint16_t data_addr = 0, w_bit_num = 0, s_data_index = 0;

    SKRMB_NO_USE(w_byte_count);

    data_addr   = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);
    w_bit_num    = SKRMB_U16_GET(dev_node->rec_buf[4], dev_node->rec_buf[5]);

    w_byte_count = dev_node->rec_buf[6];

    dev_node->send_buf[s_data_index++] = dev_node->mb_addr;
    dev_node->send_buf[s_data_index++] = SKRMB_FUNCODE_WRITE_MULTIPLE_COILS;
    dev_node->send_buf[s_data_index++] = (uint8_t)(data_addr >> 8);
    dev_node->send_buf[s_data_index++] = (uint8_t)(data_addr);
    w_bit_num = skrmb_coil_search(dev_node->reg_table, SKRMB_REG_TYPE_COIL, dev_node->reg_count, data_addr, (uint8_t *)&dev_node->rec_buf[7], w_bit_num, false);
    dev_node->send_buf[s_data_index++] = (uint8_t)(w_bit_num >> 8);
    dev_node->send_buf[s_data_index++] = (uint8_t)(w_bit_num);

    skrmb_send_data(dev_node, dev_node->send_buf, s_data_index);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_coil_read_handle(struct _skrmb_dev_node_t *dev_node)
{
    uint8_t slave_addr = 0, byte_count = 0;

    slave_addr  = dev_node->rec_buf[0];
    byte_count  = dev_node->rec_buf[2];

    skrmb_m_read_coil_cb(slave_addr, dev_node->m_wait_para->reg_addr, &dev_node->rec_buf[3], byte_count);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_coil_write_single_handle(struct _skrmb_dev_node_t *dev_node)
{
    uint8_t slave_addr = 0, coil_num = 0;
    uint16_t reg_addr = 0;

    slave_addr  = dev_node->rec_buf[0];
    reg_addr    = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);
    coil_num    = 1;

    skrmb_m_write_coil_cb(slave_addr, reg_addr, coil_num);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_coil_write_multiple_handle(struct _skrmb_dev_node_t *dev_node)
{
    uint8_t slave_addr = 0, coil_num = 0;
    uint16_t reg_addr = 0;

    slave_addr  = dev_node->rec_buf[0];
    reg_addr    = SKRMB_U16_GET(dev_node->rec_buf[2], dev_node->rec_buf[3]);
    coil_num    = SKRMB_U16_GET(dev_node->rec_buf[4], dev_node->rec_buf[5]);

    skrmb_m_write_coil_cb(slave_addr, reg_addr, coil_num);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_coil_request_read(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, uint16_t reg_num)
{
    struct _skrmb_dev_node_t *dev_node = NULL;
    
    dev_node = skrmb_find_dev(dev_id);
    SKRMB_PTR_NULL(dev_node);

    dev_node->m_wait_para->waiting_flg = true;
    dev_node->m_wait_para->smb_addr    = mb_addr;
    dev_node->m_wait_para->funcode     = SKRMB_FUNCODE_READ_COILS;
    dev_node->m_wait_para->reg_addr    = reg_addr;
    dev_node->m_wait_para->reg_num     = reg_num;
    dev_node->m_wait_para->timeout_ms  = 150;
    dev_node->m_wait_para->s_tick_ms   = skrmb_get_curr_tick_ms();

    dev_node->send_port_id             = port_id;

    skrmb_m_request_read(dev_node);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_coil_request_single_write(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, bool value)
{
    struct _skrmb_dev_node_t *dev_node = NULL;
    uint8_t on_off[] = {0xFF, 0x00};

    dev_node = skrmb_find_dev(dev_id);
    SKRMB_PTR_NULL(dev_node);

    if (!value) memset(on_off, 0x00, sizeof(on_off));

    dev_node->m_wait_para->waiting_flg = true;
    dev_node->m_wait_para->smb_addr    = mb_addr;
    dev_node->m_wait_para->funcode     = SKRMB_FUNCODE_WRITE_SINGLE_COIL;
    dev_node->m_wait_para->reg_addr    = reg_addr;
    dev_node->m_wait_para->reg_num     = 1;
    dev_node->m_wait_para->timeout_ms  = 150;
    dev_node->m_wait_para->s_tick_ms   = skrmb_get_curr_tick_ms();
    dev_node->send_port_id             = port_id;

    skrmb_m_request_write(dev_node, on_off, sizeof(on_off));

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_coil_request_multiple_write(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, uint8_t *data, uint16_t reg_num)
{
    struct _skrmb_dev_node_t *dev_node = NULL;
    uint16_t w_len = 0;

    dev_node = skrmb_find_dev(dev_id);
    SKRMB_PTR_NULL(dev_node);

    w_len = reg_num / 8;
    w_len = (reg_num % 8 == 0) ? w_len : w_len + 1;

    dev_node->m_wait_para->waiting_flg = true;
    dev_node->m_wait_para->smb_addr    = mb_addr;
    dev_node->m_wait_para->funcode     = SKRMB_FUNCODE_WRITE_MULTIPLE_COILS;
    dev_node->m_wait_para->reg_addr    = reg_addr;
    dev_node->m_wait_para->reg_num     = reg_num;
    dev_node->m_wait_para->timeout_ms  = 150;
    dev_node->m_wait_para->s_tick_ms   = skrmb_get_curr_tick_ms();
    dev_node->send_port_id             = port_id;

    skrmb_m_request_write(dev_node, data, w_len);

    return SKRMB_NO_ERROR;
}


