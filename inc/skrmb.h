#ifndef __SKRMB_H
#define __SKRMB_H

#include "skrmb_def.h"
#include "skrmb_cfg.h"
#include "skrmb_crc.h"
#include "skrmb_cmn.h"
#include "skrmb_inp.h"
#include "skrmb_intf.h"
#include "skrmb_proc.h"
#include "skrmb_reg_coil.h"
#include "skrmb_reg_hold.h"
#include "skrmb_reg_input.h"
#include "skrmb_reg_disinp.h"




extern void skrmb_dev_run(uint32_t dev_id);

extern void skrmb_mdev_wait_timeout(uint32_t dev_id);



















#endif  /* __SKRMB_H */
