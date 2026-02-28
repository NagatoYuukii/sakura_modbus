#include "skrmb_proc.h"
#include "skrmb_cmn.h"
#include "skrmb_intf.h"
#include "skrmb_reg_coil.h"
#include "skrmb_reg_disinp.h"
#include "skrmb_reg_hold.h"
#include "skrmb_reg_input.h"

static skrmb_sta_flg_e skrmb_rec_data_handle_s(struct _skrmb_dev_node_t *dev_node);
static skrmb_sta_flg_e skrmb_rec_data_handle_m(struct _skrmb_dev_node_t *dev_node);

void skrmb_rec_data_handle(uint32_t dev_id)
{
    struct _skrmb_dev_node_t *dev_node = NULL;
    
    dev_node = skrmb_find_dev(dev_id);
    SKRMB_PTR_NULL(dev_node);

    /* no data */
    if (dev_node->rec_flg == SKRMB_NO_DATA) return; 

    /* 主机从机分开处理 */
    if (dev_node->dev_role == SKRMB_ROLE_SLAVE) {
        skrmb_rec_data_handle_s(dev_node);
    } else {
        skrmb_rec_data_handle_m(dev_node);
    }

    /* clear */
    dev_node->rec_flg = SKRMB_NO_DATA;
}

static skrmb_sta_flg_e skrmb_rec_data_handle_s(struct _skrmb_dev_node_t *dev_node)
{
    uint8_t tmp_mb_addr = 0, tmp_funcode = 0;
    uint32_t data_index = 0;
    /* 不是自己的地址 */
    tmp_mb_addr = dev_node->rec_buf[data_index++];

    if (tmp_mb_addr == dev_node->broadcast_addr) {
        dev_node->rec_flg = SKRMB_DATA_BROADCAST;
    }

    if (dev_node->mb_addr != tmp_mb_addr && tmp_mb_addr != dev_node->broadcast_addr) return SKRMB_INPUT_MODBUS_ADDR_ERR;

    tmp_funcode     = dev_node->rec_buf[data_index++];

    memset(dev_node->send_buf, 0, SKRMB_DEFAULT_DATA_MAX_LEN);

    switch (tmp_funcode)
    {
    case SKRMB_FUNCODE_READ_COILS:
        skrmb_coil_read_handle(dev_node);
        break;

    case SKRMB_FUNCODE_READ_DISCRETE_INPUTS:
        skrmb_disinp_read_handle(dev_node);
        break;

    case SKRMB_FUNCODE_READ_HOLDING_REGS:
        skrmb_hold_read_handle(dev_node);
        break;

    case SKRMB_FUNCODE_READ_INPUT_REGS:
        skrmb_input_read_handle(dev_node);
        break;

    case SKRMB_FUNCODE_WRITE_SINGLE_COIL:
        skrmb_coil_write_single_handle(dev_node);
        break;

    case SKRMB_FUNCODE_WRITE_SINGLE_HOLDING_REG:
        skrmb_hold_write_single_handle(dev_node);
        break;

    case SKRMB_FUNCODE_WRITE_MULTIPLE_COILS:
        skrmb_coil_write_multiple_handle(dev_node);
        break;

    case SKRMB_FUNCODE_WRITE_MULTIPLE_HOLDING_REGS:
        skrmb_hold_write_multiple_handle(dev_node);
        break;

    default:
        break;
    }
    return SKRMB_NO_ERROR;
}

static skrmb_sta_flg_e skrmb_m_check_wait_timeout(struct _skrmb_dev_node_t *dev_node)
{
    skrmb_sta_flg_e sta_flg = SKRMB_NO_ERROR;

    if (dev_node->m_wait_para->waiting_flg && skrmb_tickcheck_ms(dev_node->m_wait_para->s_tick_ms, dev_node->m_wait_para->timeout_ms)) 
    {
        sta_flg = SKRMB_MASTER_WAIT_TIMEOUT;
    }

    if (sta_flg == SKRMB_MASTER_WAIT_TIMEOUT) 
    {
        dev_node->m_wait_para->waiting_flg = false;
        skrmb_m_wait_timeout_cb(dev_node->m_wait_para->smb_addr);
    }

    return sta_flg;
}


static skrmb_sta_flg_e skrmb_rec_data_handle_m(struct _skrmb_dev_node_t *dev_node)
{

    uint8_t tmp_mb_addr = 0, tmp_funcode = 0;
    uint32_t data_index = 0;

    /* 检查超时 */
    skrmb_m_check_wait_timeout(dev_node);

    /* 检查地址 */
    tmp_mb_addr = dev_node->rec_buf[data_index++];

    if (!dev_node->m_wait_para->waiting_flg || tmp_mb_addr != dev_node->m_wait_para->smb_addr) return SKRMB_INPUT_MODBUS_ADDR_ERR;

    tmp_funcode     = dev_node->rec_buf[data_index++];

    if (dev_node->m_wait_para->funcode != tmp_funcode) return SKRMB_INPUT_FUNC_CODE_ERR;

    switch (tmp_funcode)
    {
    case SKRMB_FUNCODE_READ_COILS:
        skrmb_m_coil_read_handle(dev_node);
        break;

    case SKRMB_FUNCODE_READ_DISCRETE_INPUTS:
        skrmb_m_disinp_read_handle(dev_node);
        break;

    case SKRMB_FUNCODE_READ_HOLDING_REGS:
        skrmb_m_hold_read_handle(dev_node);
        break;

    case SKRMB_FUNCODE_READ_INPUT_REGS:
        skrmb_m_input_read_handle(dev_node);
        break;

    case SKRMB_FUNCODE_WRITE_SINGLE_COIL:
        skrmb_m_coil_write_single_handle(dev_node);
        break;

    case SKRMB_FUNCODE_WRITE_SINGLE_HOLDING_REG:
        skrmb_m_hold_write_single_handle(dev_node);
        break;

    case SKRMB_FUNCODE_WRITE_MULTIPLE_COILS:
        skrmb_m_coil_write_multiple_handle(dev_node);
        break;

    case SKRMB_FUNCODE_WRITE_MULTIPLE_HOLDING_REGS:
        skrmb_m_hold_write_multiple_handle(dev_node);
        break;

    default:
        break;
    }

    dev_node->m_wait_para->waiting_flg = false;

    return SKRMB_NO_ERROR;
}
















