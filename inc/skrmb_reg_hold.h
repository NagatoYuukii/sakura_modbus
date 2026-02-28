#ifndef __SKRMB_REG_HOLD_H
#define __SKRMB_REG_HOLD_H

#include "skrmb_def.h"
#include "skrmb_cmn.h"

extern uint8_t skrmb_hold_data_handle(struct _skrmb_dev_reg_t *reg_table, skrmb_reg_type_e reg_type, uint16_t reg_count, uint16_t addr, uint8_t *buf, uint16_t len, bool is_read);

extern skrmb_sta_flg_e skrmb_hold_read_handle(struct _skrmb_dev_node_t *dev_node);

extern skrmb_sta_flg_e skrmb_hold_write_single_handle(struct _skrmb_dev_node_t *dev_node);

extern skrmb_sta_flg_e skrmb_hold_write_multiple_handle(struct _skrmb_dev_node_t *dev_node);

extern skrmb_sta_flg_e skrmb_m_hold_read_handle(struct _skrmb_dev_node_t *dev_node);

extern skrmb_sta_flg_e skrmb_m_hold_write_single_handle(struct _skrmb_dev_node_t *dev_node);

extern skrmb_sta_flg_e skrmb_m_hold_write_multiple_handle(struct _skrmb_dev_node_t *dev_node);

extern skrmb_sta_flg_e skrmb_m_hold_request_read(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, uint16_t reg_num);

extern skrmb_sta_flg_e skrmb_m_hold_request_single_write(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, uint16_t value);

extern skrmb_sta_flg_e skrmb_m_hold_request_multiple_write(uint32_t dev_id, uint8_t port_id, uint8_t mb_addr, uint16_t reg_addr, uint8_t *data, uint16_t data_len);

#endif  /* __SKRMB_REG_HOLD_H */
