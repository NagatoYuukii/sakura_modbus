#include "skrmb_cmn.h"
#include "skrmb_intf.h"

static struct _skrmb_dev_addr_node_t dev_addr_link = 
{
    .dev_id         = 0,
    .dev_addr       = NULL,
    .next_dev_addr  = NULL,
};

bool skrmb_tickcheck_ms(uint32_t tick, uint32_t ms)
{
    const uint32_t curTick = skrmb_get_curr_tick_ms();
    const uint32_t diff = curTick - tick;
    return diff >= ms;
}

skrmb_sta_flg_e skrmb_dev_slave_create(uint32_t dev_id, uint8_t modbus_addr, uint8_t broadcast_addr, 
                                    struct _skrmb_dev_reg_t *reg_table, uint16_t reg_count)
{
    struct _skrmb_dev_node_t *dev_node = skrmb_malloc(sizeof(skrmb_dev_node_t));
    struct _skrmb_dev_addr_node_t *dev_addr_node = skrmb_malloc(sizeof(skrmb_dev_addr_node_t));
    struct _skrmb_dev_addr_node_t *last_dev_addr_node;
    
    SKRMB_PTR_NULL(dev_node);
    SKRMB_PTR_NULL(dev_addr_node);
    SKRMB_PTR_NULL(reg_table);

    last_dev_addr_node = &dev_addr_link;
    while (last_dev_addr_node->next_dev_addr != NULL)
    {
        last_dev_addr_node = last_dev_addr_node->next_dev_addr;
    }

    /* ID Repeat */
    if (last_dev_addr_node->dev_id == dev_id) {
        skrmb_debug("dev_id repeat !\r\n");
        while (1);
    }

    dev_addr_node->dev_id   = dev_id;
    dev_addr_node->dev_addr = dev_node;
    dev_addr_node->next_dev_addr = NULL;

    last_dev_addr_node->next_dev_addr = dev_addr_node;

    dev_node->dev_role                      = SKRMB_ROLE_SLAVE;
    dev_node->first_port                    = NULL;
    dev_node->rec_buf                       = skrmb_malloc(SKRMB_DEFAULT_DATA_MAX_LEN);
    dev_node->send_buf                      = skrmb_malloc(SKRMB_DEFAULT_DATA_MAX_LEN);
    dev_node->send_port_id                  = 0;
    dev_node->rec_len                       = 0;
    dev_node->rec_flg                       = SKRMB_NO_DATA;
    dev_node->mb_addr                       = modbus_addr;
    dev_node->broadcast_addr                = broadcast_addr;
    dev_node->reg_table                     = reg_table;
    dev_node->reg_count                     = reg_count;
    dev_node->m_wait_para                   = NULL;

    SKRMB_PTR_NULL(dev_node->rec_buf);
    SKRMB_PTR_NULL(dev_node->send_buf);

    skrmb_debug("skrmb slave dev create succ dev_id:%d\r\n", dev_id);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_dev_master_create(uint32_t dev_id)
{
    struct _skrmb_dev_node_t *dev_node = skrmb_malloc(sizeof(skrmb_dev_node_t));
    struct _skrmb_dev_addr_node_t *dev_addr_node = skrmb_malloc(sizeof(skrmb_dev_addr_node_t));
    struct _skrmb_mdev_wait_para_t *wait_para = skrmb_malloc(sizeof(skrmb_mdev_wait_para_t));
    struct _skrmb_dev_addr_node_t *last_dev_addr_node;
    
    SKRMB_PTR_NULL(dev_node);
    SKRMB_PTR_NULL(dev_addr_node);
    SKRMB_PTR_NULL(wait_para);

    last_dev_addr_node = &dev_addr_link;
    while (last_dev_addr_node->next_dev_addr != NULL)
    {
        last_dev_addr_node = last_dev_addr_node->next_dev_addr;
    }

    /* ID Repeat */
    if (last_dev_addr_node->dev_id == dev_id) {
        skrmb_debug("dev_id repeat !\r\n");
        while (1);
    }

    dev_addr_node->dev_id   = dev_id;
    dev_addr_node->dev_addr = dev_node;
    dev_addr_node->next_dev_addr = NULL;

    last_dev_addr_node->next_dev_addr = dev_addr_node;

    wait_para->waiting_flg                  = false;
    wait_para->smb_addr                     = 0;
    wait_para->funcode                      = 0;
    wait_para->reg_addr                     = 0;
    wait_para->reg_num                      = 0;
    wait_para->timeout_ms                   = 0;

    dev_node->dev_role                      = SKRMB_ROLE_SLAVE;
    dev_node->first_port                    = NULL;
    dev_node->rec_buf                       = skrmb_malloc(SKRMB_DEFAULT_DATA_MAX_LEN);
    dev_node->send_buf                      = skrmb_malloc(SKRMB_DEFAULT_DATA_MAX_LEN);
    dev_node->send_port_id                  = 0;
    dev_node->rec_len                       = 0;
    dev_node->rec_flg                       = SKRMB_NO_DATA;
    dev_node->mb_addr                       = 0;
    dev_node->broadcast_addr                = 0;
    dev_node->reg_table                     = NULL;
    dev_node->reg_count                     = 0;
    dev_node->m_wait_para                   = wait_para;

    SKRMB_PTR_NULL(dev_node->rec_buf);
    SKRMB_PTR_NULL(dev_node->send_buf);

    skrmb_debug("skrmb master dev create succ dev_id:%d\r\n", dev_id);

    return SKRMB_NO_ERROR;
}

struct _skrmb_dev_node_t *skrmb_find_dev(uint32_t dev_id)
{
    struct _skrmb_dev_node_t *dev_addr = NULL;
    struct _skrmb_dev_addr_node_t *last_dev_addr_node;

    last_dev_addr_node = &dev_addr_link;
    while ( last_dev_addr_node != NULL)
    {
        // skrmb_debug("last_dev_addr_node->dev_id = %d\r\n", last_dev_addr_node->dev_id);
        if (last_dev_addr_node->dev_id == dev_id) {
            dev_addr = last_dev_addr_node->dev_addr;
            break;
        }
        last_dev_addr_node = last_dev_addr_node->next_dev_addr;
    }
    return dev_addr;
}

skrmb_sta_flg_e skrmb_dev_add_port(uint32_t dev_id, uint32_t port_id, void(* send_func)(uint8_t *d, uint16_t len))
{
    struct _skrmb_dev_port_t *dev_port = skrmb_malloc(sizeof(skrmb_dev_port_t));
    struct _skrmb_dev_node_t *dev_addr = NULL;
    struct _skrmb_dev_port_t *last_dev_port = NULL;

    SKRMB_PTR_NULL(send_func);
    SKRMB_PTR_NULL(dev_port);

    dev_addr = skrmb_find_dev(dev_id);
    SKRMB_PTR_NULL(dev_addr);

    last_dev_port = dev_addr->first_port;
    if (dev_addr->first_port == NULL) {
        dev_addr->first_port = dev_port;
        // skrmb_debug("dev_addr->first_port->port_id = %d\r\n", dev_addr->first_port->port_id);
    } else {
        while (1) 
        {
            /* PORT_ID REPEAT */
            // skrmb_debug("last_dev_port->port_id = %d\r\n", last_dev_port->port_id);
            if (last_dev_port->port_id == port_id) SKRMB_PTR_NULL(NULL);
            if (last_dev_port->next_port_addr == NULL) break;
            last_dev_port = last_dev_port->next_port_addr;
        }
        last_dev_port->next_port_addr = dev_port;
    }

    dev_port->port_id           = port_id;
    dev_port->port_send_func    = send_func;
    dev_port->port_send_s_tick_ms = 0;
    dev_port->next_port_addr    = NULL;

    skrmb_debug("skrmb add port succ port_id:%d\r\n", port_id);

    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_send_data(struct _skrmb_dev_node_t *dev_node, uint8_t *data, uint16_t len)
{
    struct _skrmb_dev_port_t *send_port = NULL;

#if (!SKRMB_CONFIG_BROADCAST_RESP)
    // 广播数据不回复
    if (dev_node->rec_flg == SKRMB_DATA_BROADCAST && dev_node->dev_role == SKRMB_ROLE_SLAVE) return SKRMB_NO_ERROR;
#endif
    send_port = skrmb_find_port(dev_node, dev_node->send_port_id);
    SKRMB_PTR_NULL(send_port);
    // 一般为3.5个字符
    while (!skrmb_tickcheck_ms(send_port->port_send_s_tick_ms, SKRMB_DEFAULT_SEND_GAP_MS)) skrmb_delay_ms(10);
    send_port->port_send_func(data, len);
    send_port->port_send_s_tick_ms = skrmb_get_curr_tick_ms();
    
    return SKRMB_NO_ERROR;
}

struct _skrmb_dev_port_t *skrmb_find_port(struct _skrmb_dev_node_t *dev_node, uint8_t port_id)
{
    struct _skrmb_dev_port_t *dev_port = NULL;
    struct _skrmb_dev_port_t *last_dev_port_node;

    SKRMB_PTR_NULL(dev_node);

    last_dev_port_node = dev_node->first_port;
    while ( last_dev_port_node != NULL)
    {
        if (last_dev_port_node->port_id == port_id) {
            dev_port = last_dev_port_node;
            break;
        }
        last_dev_port_node = last_dev_port_node->next_port_addr;
    }
    return dev_port;
}

uint16_t skrmb_err_response(struct _skrmb_dev_node_t *dev_node, uint8_t funcode, skrmb_err_resp_e err_type)
{
    uint16_t s_data_index = 0;

    dev_node->send_buf[s_data_index++] = dev_node->mb_addr;
    dev_node->send_buf[s_data_index++] = SKRMB_FUNCODE_IS_EXCEPTION(funcode);
    dev_node->send_buf[s_data_index++] = err_type;

    return s_data_index;
}

skrmb_sta_flg_e skrmb_m_request_read(struct _skrmb_dev_node_t *dev_node)
{
    uint16_t s_data_index = 0, tmp_crc = 0;

    dev_node->send_buf[s_data_index++] = dev_node->m_wait_para->smb_addr;
    dev_node->send_buf[s_data_index++] = dev_node->m_wait_para->funcode;
    dev_node->send_buf[s_data_index++] = (uint8_t)(dev_node->m_wait_para->reg_addr >> 8);
    dev_node->send_buf[s_data_index++] = (uint8_t)(dev_node->m_wait_para->reg_addr);
    dev_node->send_buf[s_data_index++] = (uint8_t)(dev_node->m_wait_para->reg_num >> 8);
    dev_node->send_buf[s_data_index++] = (uint8_t)(dev_node->m_wait_para->reg_num);

    tmp_crc = skrmb_crc(dev_node->send_buf, s_data_index);
    dev_node->send_buf[s_data_index++] = (uint8_t)tmp_crc;
    dev_node->send_buf[s_data_index++] = (uint8_t)(tmp_crc >> 8);

    skrmb_send_data(dev_node, dev_node->send_buf, s_data_index);
    return SKRMB_NO_ERROR;
}

skrmb_sta_flg_e skrmb_m_request_write(struct _skrmb_dev_node_t *dev_node, void *w_data, uint16_t w_len)
{
    uint16_t s_data_index = 0, tmp_crc = 0;

    dev_node->send_buf[s_data_index++] = dev_node->m_wait_para->smb_addr;
    dev_node->send_buf[s_data_index++] = dev_node->m_wait_para->funcode;
    dev_node->send_buf[s_data_index++] = (uint8_t)(dev_node->m_wait_para->reg_addr >> 8);
    dev_node->send_buf[s_data_index++] = (uint8_t)(dev_node->m_wait_para->reg_addr);
    if (dev_node->m_wait_para->funcode == SKRMB_FUNCODE_WRITE_MULTIPLE_COILS \
    || dev_node->m_wait_para->funcode == SKRMB_FUNCODE_WRITE_MULTIPLE_HOLDING_REGS)
    {
        dev_node->send_buf[s_data_index++] = (uint8_t)(dev_node->m_wait_para->reg_num >> 8);
        dev_node->send_buf[s_data_index++] = (uint8_t)(dev_node->m_wait_para->reg_num);
        dev_node->send_buf[s_data_index++] = (uint8_t)w_len;
    }
    memcpy(&dev_node->send_buf[s_data_index], w_data, w_len);
    s_data_index += w_len;

    tmp_crc = skrmb_crc(dev_node->send_buf, s_data_index);
    dev_node->send_buf[s_data_index++] = (uint8_t)tmp_crc;
    dev_node->send_buf[s_data_index++] = (uint8_t)(tmp_crc >> 8);

    skrmb_send_data(dev_node, dev_node->send_buf, s_data_index);
    return SKRMB_NO_ERROR;
}




