# Sakura Modbus 协议栈

这是一个modbus协议栈 (> _ <) !  
为什么要写这个东西：
* 因为FreeModbus无法在协议栈层面进行数据跟踪输入输出，要实现多端口改动太大。
* FreeModbus的涉及了底层硬件的初始化和配置，感觉不太好
* FreeModbus无法一个MCU设备支持多个地址
* FreeModbus寄存器地址需要连续(导致我MCU一下要开一个大内存，导致项目内存有点浪费)
(还有一些忘了，反正就是有点不太适合我的项目，FreeModbus能用但是可以更好用一点)  

因为以上原因，决定重新编写了一套自己的轮子。

全称Sakura Modbus协议栈，简称skrmb(收款人民币...)  
目前已经实现功能：  
* 支持主机和从机，从机模式无需操作系统支持，主机模式需要操作系统支持。  
* 数据格式支持Modbus RTU
* 支持端口注册，可以一个设备多个端口，协议栈自动跟踪，从哪里来就从哪里出
* 支持注册多个从机地址

使用方法：  
1. 把src的.c和inc的.h全部丢到工程中去。
2. 如果需要使用从机设备，需要构建```struct _skrmb_dev_reg_t```数组，下一步的注册需要传入。
3. 使用``` skrmb_dev_slave_create() ```或者``` skrmb_dev_master_create() ```注册设备，每一个设备id都是唯一的，重复注册相同的设备id是不允许的。
4. 使用```skrmb_dev_add_port() ```对设备进行端口注册, 在相同设备id下，重复注册相同id的端口号也是不允许的。但是不同设备id可以存在相同的端口号。
5. 把```skrmb_dev_run(dev_id)```丢到程序while循环运行就行了。
6. 去 ` src/skrmb_intf.c `中实现里面函数

这里详细说一下` src/skrmb_intf.c `里面的函数有什么用  
```
/* 内存分配函数必须要实现，只在一开始注册设备和端口时会用到，在运行过错中无分配内存 */
extern void *skrmb_malloc(uint32_t size);
/* 内存释放函数必须要实现，但是目前没有地方使用到释放函数，因为申请的内存需要一直存活 */
extern void skrmb_free(void *ptr);
/* crc函数必须实现 */ 
extern uint16_t skrmb_crc(uint8_t *data, uint16_t len);
/* (必须实现)获取一个ms级的tick，可以使用定时器实现，也可以使用rtos的tick */
extern uint32_t skrmb_get_curr_tick_ms(void);
/* (可选)延时函数，在发送端口未到3.5字符时会进行延时，如果不实现的话就无延时 */
extern void skrmb_delay_ms(uint32_t ms);
/* (从机不需要) 下面都是主机收到回复后的回调函数 */
extern void skrmb_m_read_coil_cb(uint8_t mb_addr, uint16_t reg_addr, uint8_t *coil_data, uint16_t byte_len);
extern void skrmb_m_read_disinp_cb(uint8_t mb_addr, uint16_t reg_addr, uint8_t *disinp_data, uint16_t byte_len);
extern void skrmb_m_write_coil_cb(uint8_t mb_addr, uint16_t reg_addr, uint16_t coil_num);
extern void skrmb_m_read_hold_cb(uint8_t mb_addr, uint16_t reg_addr, uint8_t *hold_data, uint16_t byte_len);
extern void skrmb_m_read_input_cb(uint8_t mb_addr, uint16_t reg_addr, uint8_t *input_data, uint16_t byte_len);
extern void skrmb_m_write_hold_cb(uint8_t mb_addr, uint16_t reg_addr, uint16_t write_reg_num);
/* (从机不需要) 等待从机回复超时回调*/
extern void skrmb_m_wait_timeout_cb(uint8_t mb_addr);
```

代码结构：
```
.
├── bin
│   └── sakura_modbus           (测试用例程序)
├── inc                         (头文件文件夹)
│   ├── skrmb_cfg.h             (配置文件,开启或者关闭一些功能)
│   ├── skrmb_cmn.h             (协议栈通用内容头文件)
│   ├── skrmb_crc.h             (协议栈标准crc函数头文件)
│   ├── skrmb_def.h             (协议栈通用定义头文件)
│   ├── skrmb.h                 (协议栈总头文件)
│   ├── skrmb_inp.h             (输入接口描述头文件)
│   ├── skrmb_intf.h            (外部接口声明头文件)
│   ├── skrmb_proc.h            (总进程头文件)
│   ├── skrmb_reg_coil.h        (线圈寄存器相关头文件)
│   ├── skrmb_reg_disinp.h      (离散输入寄存器相关头文件)
│   ├── skrmb_reg_hold.h        (保持寄存器相关头文件)
│   └── skrmb_reg_input.h       (输入寄存器相关头文件)
├── main.c                      (测试用例代码)
├── main.h                      (测试用例头文件)
├── Makefile
├── obj                         (这个文件夹内的都是编译产生的中间文件)
│   ├── main.o
│   ├── skrmb_cmn.o
│   ├── skrmb_crc.o
│   ├── skrmb_inp.o
│   ├── skrmb_intf.o
│   ├── skrmb.o
│   ├── skrmb_proc.o
│   ├── skrmb_reg_coil.o
│   ├── skrmb_reg_disinp.o
│   ├── skrmb_reg_hold.o
│   └── skrmb_reg_input.o
├── Readme.md                   (self)
└── src                         (源文件文件夹)
    ├── Makefile                (子Makefile)
    ├── skrmb.c                 (协议栈总调用)
    ├── skrmb_cmn.c             (协议栈通用内容)
    ├── skrmb_crc.c             (标准crc)
    ├── skrmb_inp.c             (输入接口实现)
    ├── skrmb_intf.c            (外部接口实现)
    ├── skrmb_proc.c            (总进程处理)
    ├── skrmb_reg_coil.c        (线圈寄存器相关处理)
    ├── skrmb_reg_disinp.c      (离散输入寄存器相关处理)
    ├── skrmb_reg_hold.c        (保持寄存器相关处理)
    └── skrmb_reg_input.c       (输入寄存器相关处理)
```

