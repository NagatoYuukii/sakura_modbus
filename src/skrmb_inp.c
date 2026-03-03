#include "skrmb_inp.h"
#include "skrmb_cmn.h"
#include "skrmb_intf.h"

skrmb_sta_flg_e skrmb_data_push(uint32_t dev_id, uint8_t port_id, uint8_t *data, uint16_t len)
{
    struct _skrmb_dev_node_t *dev_node = NULL;
    struct _skrmb_dev_port_t *send_port = NULL;
    uint16_t tmp_crc = 0, tmp_trans_id = 0, tmp_byte_len = 0;

    if (len > SKRMB_DEFAULT_DATA_MAX_LEN || len < SKRMB_DEFAULT_DATA_MIN_LEN) return SKRMB_INPUT_DATA_ERR;

    dev_node = skrmb_find_dev(dev_id);

    if (dev_node == NULL) return SKRMB_DEV_NO_FIND;
    if (dev_node->rec_flg != SKRMB_NO_DATA) return SKRMB_INPUT_DATA_FULL;

    send_port = skrmb_find_port(dev_node, port_id);
    SKRMB_PTR_NULL(send_port);

    if (send_port->port_type == SKRMB_RTU_PORT)
    {
        tmp_crc = skrmb_crc(data, len - 2);
        if (tmp_crc != (data[len - 1] << 8 | data[len - 2])) return SKRMB_INPUT_DATA_ERR;
    }
    else if (send_port->port_type == SKRMB_TCP_PORT)
    {
        tmp_trans_id = SKRMB_U16_GET(data[0], data[1]);
        tmp_byte_len = SKRMB_U16_GET(data[4], data[5]);
        // tcp check data
        if (tmp_byte_len != (len - 6)) return SKRMB_INPUT_DATA_ERR;
        dev_node->rec_trans_id = tmp_trans_id;
        data += 6;  // offset to unit id
        len -= 6;
    }

    memcpy(dev_node->rec_buf, data, len);
    dev_node->rec_len     = len;
    dev_node->send_port_id = port_id;
    dev_node->rec_flg     = SKRMB_DATA_NORMAL;

    return SKRMB_NO_ERROR;
}




















