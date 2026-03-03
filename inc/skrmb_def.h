#ifndef __SKRMB_DEF_H
#define __SKRMB_DEF_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SKRMB_DEFAULT_BROADCAST_ADDR                    (0xFF)
#define SKRMB_DEFAULT_DATA_MIN_LEN                      (4)
#define SKRMB_DEFAULT_DATA_MAX_LEN                      (256)
#define SKRMB_DEFAULT_SEND_GAP_MS                       (0)             // send data gap

/* low/high bit get value */
#define SKRMB_U16_GET(h,l)                             ((h << 8) | l)

/* tcp mode def */
#define SKRMB_DEFAULT_TCP_PROTOCOL_ID                   (0x0000)

/* stack support function code */
#define SKRMB_FUNCODE_READ_COILS                    (0x01U) // read coil register（01H）
#define SKRMB_FUNCODE_READ_DISCRETE_INPUTS          (0x02U) // read discrete input register（02H）
#define SKRMB_FUNCODE_READ_HOLDING_REGS             (0x03U) // read hold register（03H）
#define SKRMB_FUNCODE_READ_INPUT_REGS               (0x04U) // read input register（04H）
#define SKRMB_FUNCODE_WRITE_SINGLE_COIL             (0x05U) // write single coil register（05H）
#define SKRMB_FUNCODE_WRITE_SINGLE_HOLDING_REG      (0x06U) // write single hold register（06H）
#define SKRMB_FUNCODE_WRITE_MULTIPLE_COILS          (0x0FU) // write multiple coil register（0FH/15）
#define SKRMB_FUNCODE_WRITE_MULTIPLE_HOLDING_REGS   (0x10U) // write multiple hold register（10H/16）

/* function err flag（standard: original function code + 0x80）*/
#define SKRMB_FUNCODE_EXCEPTION_MASK                (0x80U)
#define SKRMB_FUNCODE_IS_EXCEPTION(func_code) ((func_code) & SKRMB_FUNCODE_EXCEPTION_MASK)

/* stack debug printf */
#define skrmb_debug(format, ...)                     printf(format,  ## __VA_ARGS__)
#define skrmb_debug_array(data, data_len)   \
        do { \
            skrmb_debug("["); \
            for (uint32_t i = 0; i < data_len; i++) \
            { \
                if (i % 32 == 0 && i != 0) skrmb_debug("\r\n"); \
                skrmb_debug("%02X", (data)[i]); \
            } \
            skrmb_debug("]\r\n"); \
        } while (0)

/* stack sta return */
enum
{
    SKRMB_NO_ERROR = 0,                                     // no error
    SKRMB_DEV_NO_FIND,                                      // device id not found
    SKRMB_PORT_NO_FIND,                                     // port id not found
    SKRMB_INPUT_DATA_ERR,                                   // input data length or crc exception
    SKRMB_INPUT_DATA_FULL,                                  // input data processing area full
    SKRMB_INPUT_MODBUS_ADDR_ERR,                            // input modbus address exception
    SKRMB_INPUT_FUNC_CODE_ERR,                              // invalid function code
    SKRMB_INPUT_TCP_TRANS_ID_ERR,                           // TCP mode trans id err
    SKRMB_REG_INDEX_OVERFLOW,                               // data index to read or write does not exist
    SKRMB_MASTER_WAIT_TIMEOUT,                              // master wait timeout
};
typedef uint8_t skrmb_sta_flg_e;

/* stack dev role type */
enum 
{
    SKRMB_ROLE_SLAVE = 0,   // slave
    SKRMB_ROLE_MASTER,      // master
};
typedef uint8_t skrmb_role_e;

/* reg type */
enum {
    SKRMB_REG_TYPE_HOLDING = 0,          // modbus holding register (function code 03/16, read/write, used for core business)
    SKRMB_REG_TYPE_INPUT,                // modbus input register (function code 04, read only, e.g. sensor collected data)
    SKRMB_REG_TYPE_COIL,                 // modbus coil register (function code 01/05, boolean type, read/write, e.g. switch status)
    SKRMB_REG_TYPE_DISCRETE_INPUT,       // modbus discrete input register (function code 02, read only boolean, e.g. fault contact)
};
typedef uint8_t skrmb_reg_type_e;

/* data err description */
enum 
{
    SKRMB_ERR_ILLEGAL_FUNC     = 0x01,  // 01: illegal function code (the slave does not support the function code requested by the master)
    SKRMB_ERR_ILLEGAL_DADDR    = 0x02,  // 02: illegal data address (the requested register/discrete input address does not exist or is out of range)
    SKRMB_ERR_ILLEGAL_DAVALUE  = 0x03,  // 03: illegal data value
    SKRMB_ERR_SLAVE_DEV_FAIL   = 0x04,  // 04: slave device failure (internal failure of the slave, unable to execute the request)
    SKRMB_ERR_ACK              = 0x05,  // 05: acknowledge (the slave receives the request and needs delayed processing, the master must wait)
    SKRMB_ERR_SLAVE_DEV_BUSY   = 0x06,  // 06: slave device busy (the slave is processing other requests and cannot respond temporarily)
    SKRMB_ERR_MEM_PARITY_ERR   = 0x08,  // 08: memory parity error (data verification of the slave memory area failed)
    SKRMB_ERR_GATEWAY_PATH     = 0x0A,  // 10: gateway path unavailable (the gateway cannot find the path to the target slave)
    SKRMB_ERR_GATEWAY_TARGET   = 0x0B,  // 11: gateway target device no response (the gateway finds the path, but the target slave does not respond)
};
typedef uint8_t skrmb_err_resp_e;

/* data type */
enum
{
    SKRMB_NO_DATA = 0,                  // no data
    SKRMB_DATA_NORMAL,                  // normal data
    SKRMB_DATA_BROADCAST,               // broadcast data
};
typedef uint8_t skrmb_data_type_e;

/* port type */
enum
{
    SKRMB_RTU_PORT = 0,
    SKRMB_TCP_PORT,
};
typedef uint8_t skrmb_port_type_e;

#endif  /* __SKRMB_DEF_H */
