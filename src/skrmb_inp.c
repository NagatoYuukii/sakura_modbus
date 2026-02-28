#include "skrmb_inp.h"
#include "skrmb_cmn.h"
#include "skrmb_intf.h"

skrmb_sta_flg_e skrmb_data_push(uint32_t dev_id, uint8_t port_id, uint8_t *data, uint16_t len)
{
    struct _skrmb_dev_node_t *dev_node = NULL;
    uint16_t tmp_crc = 0;
    
    if (len > SKRMB_DEFAULT_DATA_MAX_LEN || len < SKRMB_DEFAULT_DATA_MIN_LEN) return SKRMB_INPUT_DATA_ERR;
    tmp_crc = skrmb_crc(data, len - 2);
    if (tmp_crc != (data[len - 1] << 8 | data[len - 2])) return SKRMB_INPUT_DATA_ERR;

    dev_node = skrmb_find_dev(dev_id);

    if (dev_node == NULL) return SKRMB_DEV_NO_FIND;
    if (dev_node->rec_flg != SKRMB_NO_DATA) return SKRMB_INPUT_DATA_FULL;

    memcpy(dev_node->rec_buf, data, len);
    dev_node->rec_len     = len;
    dev_node->send_port_id = port_id;
    dev_node->rec_flg     = SKRMB_DATA_NORMAL;

    return SKRMB_NO_ERROR;
}




















