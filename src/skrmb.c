#include "skrmb.h"


void skrmb_dev_run(uint32_t dev_id)
{
    skrmb_rec_data_handle(dev_id);
}

void skrmb_mdev_wait_timeout(uint32_t dev_id)
{
    struct _skrmb_dev_node_t *dev_node = NULL;
    
    dev_node = skrmb_find_dev(dev_id);
    SKRMB_PTR_NULL(dev_node);

    while(dev_node->m_wait_para->waiting_flg) skrmb_delay_ms(10);
}


















