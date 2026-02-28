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
#define SKRMB_DEFAULT_SEND_GAP_MS                       (0)

/* 高低位取值 */
#define SKRMB_U16_GET(h,l)                             ((h << 8) | l)

/* 协议栈支持的功能码 */
#define SKRMB_FUNCODE_READ_COILS                    (0x01U) // 读线圈寄存器（01H）
#define SKRMB_FUNCODE_READ_DISCRETE_INPUTS          (0x02U) // 读离散输入寄存器（02H）
#define SKRMB_FUNCODE_READ_HOLDING_REGS             (0x03U) // 读保持寄存器（03H）
#define SKRMB_FUNCODE_READ_INPUT_REGS               (0x04U) // 读输入寄存器（04H）
#define SKRMB_FUNCODE_WRITE_SINGLE_COIL             (0x05U) // 写单个线圈寄存器（05H）
#define SKRMB_FUNCODE_WRITE_SINGLE_HOLDING_REG      (0x06U) // 写单个保持寄存器（06H）
#define SKRMB_FUNCODE_WRITE_MULTIPLE_COILS          (0x0FU) // 写多个线圈寄存器（0FH/15）
#define SKRMB_FUNCODE_WRITE_MULTIPLE_HOLDING_REGS   (0x10U) // 写多个保持寄存器（10H/16）

/* 功能码异常响应标识（标准：原功能码+0x80）*/
#define SKRMB_FUNCODE_EXCEPTION_MASK                (0x80U)
#define SKRMB_FUNCODE_IS_EXCEPTION(func_code) ((func_code) & SKRMB_FUNCODE_EXCEPTION_MASK)

/* 输出定义*/
#define skrmb_debug(format, ...)                     printf(format,  ## __VA_ARGS__)

/* 协议栈状态返回描述 */
enum
{
    SKRMB_NO_ERROR = 0,                                     // 无错误
    SKRMB_DEV_NO_FIND,                                      // 未找到设备id
    SKRMB_INPUT_DATA_ERR,                                   // 输入数据长度或crc异常
    SKRMB_INPUT_DATA_FULL,                                  // 输入数据处理区满
    SKRMB_INPUT_MODBUS_ADDR_ERR,                            // 输入modbus地址异常
    SKRMB_INPUT_FUNC_CODE_ERR,                              // 无效功能码
    SKRMB_REG_INDEX_OVERFLOW,                               // 需要读取或写入的数据索引不存在
    SKRMB_MASTER_WAIT_TIMEOUT,                              // 主机等待超时
};
typedef uint8_t skrmb_sta_flg_e;

/* 设备角色 */
enum 
{
    SKRMB_ROLE_SLAVE = 0,   // 从机角色
    SKRMB_ROLE_MASTER,      // 主机角色
};
typedef uint8_t skrmb_role_e;

/* 寄存器数据类型 */
enum {
    SKRMB_REG_TYPE_HOLDING = 0,          // Modbus保持寄存器（03/16功能码，可读可写，核心业务用）
    SKRMB_REG_TYPE_INPUT,                // Modbus输入寄存器（04功能码，只读，如传感器采集数据）
    SKRMB_REG_TYPE_COIL,                 // Modbus线圈寄存器（01/05功能码，布尔型，可读可写，如开关状态）
    SKRMB_REG_TYPE_DISCRETE_INPUT,       // Modbus离散输入寄存器（02功能码，只读布尔，如故障触点）
};
typedef uint8_t skrmb_reg_type_e;

/* 数据异常描述*/
enum 
{
    SKRMB_ERR_ILLEGAL_FUNC     = 0x01,  // 01：非法功能码（从站不支持主站请求的功能码）
    SKRMB_ERR_ILLEGAL_DADDR    = 0x02,  // 02：非法数据地址（请求的寄存器/离散输入地址不存在/超出范围）
    SKRMB_ERR_ILLEGAL_DAVALUE  = 0x03,  // 03：非法数据值
    SKRMB_ERR_SLAVE_DEV_FAIL   = 0x04,  // 04：从站设备故障（从站内部故障，无法执行请求）
    SKRMB_ERR_ACK              = 0x05,  // 05：确认（从站接收请求，需要延时处理，主站需等待）
    SKRMB_ERR_SLAVE_DEV_BUSY   = 0x06,  // 06：从站设备忙（从站正在处理其他请求，暂时无法响应）
    SKRMB_ERR_MEM_PARITY_ERR   = 0x08,  // 08：存储区奇偶性错误（从站存储区数据校验失败）
    SKRMB_ERR_GATEWAY_PATH     = 0x0A,  // 10：网关路径不可用（网关无法找到目标从站路径）
    SKRMB_ERR_GATEWAY_TARGET   = 0x0B,  // 11：网关目标设备无响应（网关找到路径，但目标从站未响应）
};
typedef uint8_t skrmb_err_resp_e;

/* 数据接收类型 */
enum
{
    SKRMB_NO_DATA = 0,                  // 无数据
    SKRMB_DATA_NORMAL,                  // 正常数据
    SKRMB_DATA_BROADCAST,               // 广播数据
};
typedef uint8_t skrmb_data_type_e;




#endif  /* __SKRMB_DEF_H */
