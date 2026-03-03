#ifndef __SKRMB_CMN_H
#define __SKRMB_CMN_H

#include "skrmb_def.h"
#include "skrmb_cfg.h"

/* check the pointer is null */
#define SKRMB_PTR_NULL(ptr)               \
    do {                                  \
        if ((ptr) == NULL) {              \
            skrmb_debug("[SKRMB_ASSET ERROR] NULL pointer: %s\r\n",#ptr);         \
            while (1);                    \
        }                                 \
    } while (0)

/* no used handle */
#define SKRMB_NO_USE(data) (void)(data)

/* dev port description */
typedef struct _skrmb_dev_port_t
{
    uint8_t                 port_id;
    skrmb_port_type_e       port_type;
    void(* port_send_func)(uint8_t *d, uint16_t len);
    uint32_t                port_send_s_tick_ms;
    struct _skrmb_dev_port_t *next_port_addr;
}skrmb_dev_port_t;

/* dev reg description */
typedef struct _skrmb_dev_reg_t
{
    skrmb_reg_type_e reg_type;                          /* reg data type */
    uint16_t start_addr;                                /* reg data start addr */
    uint16_t reg_len;                                   /* total len/total bit（When it is a coil, it represents the total number of bit） */
    void     *reg_data;                                 /* reg data pointer */
}skrmb_dev_reg_t;

/* master dev wait response para */
typedef struct _skrmb_mdev_wait_para_t
{
    bool                        waiting_flg;            /* wait response flag */
    uint16_t                    transaction_id;         /* tcp master transaction id */
    uint8_t                     smb_addr;               /* wait slave modbus addr */
    uint8_t                     funcode;                /* wait function code */
    uint16_t                    reg_addr;               /* wait reg start addr */
    uint16_t                    reg_num;                /* wait reg num */
    uint32_t                    timeout_ms;             /* wait timeout count(ms) */
    uint32_t                    s_tick_ms;              /* start wait tick record */
}skrmb_mdev_wait_para_t;

/* dev node */
typedef struct _skrmb_dev_node_t
{
    skrmb_role_e                dev_role;                 /* role(slave / master) */
    struct _skrmb_dev_port_t    *first_port;              /* port table addr */
    uint8_t                     mb_addr;                  /* dev modbus addr */
    uint8_t                     broadcast_addr;           /* dev broadcast addr */
    uint8_t                     *rec_buf;                 /* recv buf addr */
    uint16_t                    rec_len;                  /* recv data len */
    uint16_t                    rec_trans_id;             /* tcp mode trans id */
    skrmb_data_type_e           rec_flg;                  /* data recv type */
    uint8_t                     send_port_id;             /* data send port id */ 
    uint8_t                     *send_buf;                /* send buf addr */
    struct _skrmb_dev_reg_t     *reg_table;               /* dev reg table addr */
    uint16_t                    reg_count;                /* reg table element num */
    struct _skrmb_mdev_wait_para_t *m_wait_para;          /* if dev is master/wait resp para */
}skrmb_dev_node_t;

/* dev linked list */
typedef struct _skrmb_dev_addr_node_t
{
    uint32_t                        dev_id;             /* dev id */
    skrmb_dev_node_t                *dev_addr;          /* dev node addr */
    struct _skrmb_dev_addr_node_t   *next_dev_addr;     /* next dev node addr */
}skrmb_dev_addr_node_t;

extern bool skrmb_tickcheck_ms(uint32_t tick, uint32_t ms);

extern uint16_t skrmb_return_master_transaction_id(struct _skrmb_dev_node_t *dev_node);

extern skrmb_sta_flg_e skrmb_reverse_two_bytes(uint8_t *data, uint16_t data_len);

extern uint16_t skrmb_err_response(struct _skrmb_dev_node_t *dev_node, uint8_t funcode, skrmb_err_resp_e err_type);

extern skrmb_sta_flg_e skrmb_m_request_read(struct _skrmb_dev_node_t *dev_node);

extern skrmb_sta_flg_e skrmb_m_request_write(struct _skrmb_dev_node_t *dev_node, void *w_data, uint16_t w_len);

extern struct _skrmb_dev_node_t *skrmb_find_dev(uint32_t dev_id);

extern struct _skrmb_dev_port_t *skrmb_find_port(struct _skrmb_dev_node_t *dev_node, uint8_t port_id);

extern skrmb_sta_flg_e skrmb_send_data(struct _skrmb_dev_node_t *dev_node, uint8_t *data, uint16_t len);

extern skrmb_sta_flg_e skrmb_dev_slave_create(uint32_t dev_id, uint8_t modbus_addr, uint8_t broadcast_addr, struct _skrmb_dev_reg_t *reg_table, uint16_t reg_count);

extern skrmb_sta_flg_e skrmb_dev_master_create(uint32_t dev_id);

extern skrmb_sta_flg_e skrmb_dev_add_port(uint32_t dev_id, uint32_t port_id, void(* send_func)(uint8_t *d, uint16_t len), skrmb_port_type_e port_type);











#endif  /* __SKRMB_CMN_H */
