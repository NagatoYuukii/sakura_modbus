#ifndef __SKRMB_REG_INPUT_H
#define __SKRMB_REG_INPUT_H

#include "skrmb_def.h"
#include "skrmb_cmn.h"


extern skrmb_sta_flg_e skrmb_input_read_handle(struct _skrmb_dev_node_t *dev_node);

extern skrmb_sta_flg_e skrmb_m_input_read_handle(struct _skrmb_dev_node_t *dev_node);

extern skrmb_sta_flg_e skrmb_m_input_request_read(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, uint16_t reg_num);




#endif  /* __SKRMB_REG_INPUT_H */
