#include "main.h"
#include <unistd.h>

#define Printf_array(data, data_len)   \
        do { \
            printf("["); \
            for (uint32_t i = 0; i < data_len; i++) \
            { \
                if (i % 32 == 0 && i != 0) printf("\r\n"); \
                printf("%02X", (data)[i]); \
            } \
            printf("]\r\n"); \
        } while (0)

/* coil test */
uint8_t read_coil_buf[] = {0x01, 0x01, 0x00, 0x01, 0x00, 0x0A, 0xED, 0xCD};
uint8_t write_coil_single[] = {0x01, 0x05, 0x00, 0x06, 0xFF, 0x00, 0x6C, 0x3B};
uint8_t write_coil_mul[] = {0x01, 0x0F, 0x00, 0x00, 0x00, 0x0A, 0x02, 0x00, 0x00, 0xE5, 0x38};
/* coil_master */
uint8_t coil_w[] = {0xAA, 0x01};
/* hold test */
uint8_t read_hold[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0xC5, 0xCD};
uint8_t write_hold_single[] = {0x01, 0x06, 0x00, 0x00, 0x00, 0xFF, 0xC9, 0x8A};
uint8_t write_hold_mul[] = {0x01, 0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0xFF, 0xB3, 0xEF};
/* hold master */
// uint8_t hold_w[] = {0xbb, 0xcc, 0xaa};
uint16_t hold_w[] = {0xBBCC, 0x00AA};
/* input test */
uint8_t read_input[] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x0A, 0x70, 0x0D};
/* dis input test */
uint8_t read_dis_input[] = {0x01, 0x02, 0x00, 0x00, 0x00, 0x0A, 0xF8, 0x0D};


uint8_t hold_reg[] = {0x01, 0x00, 0x01, 0x16};
uint8_t input_reg[] = {0x01, 0x00, 0x01, 0x01, 0x00, 0x01};
bool    coil_reg = true;
bool    disinp   = true;
uint8_t coil_reg_m[] = {0x01, 0x01};

static struct _skrmb_dev_reg_t modbus_slave_reg[] =
{
    {SKRMB_REG_TYPE_COIL,                   0x0000, 1,                  &coil_reg},
    {SKRMB_REG_TYPE_COIL,                   0x0001, 9,                  coil_reg_m},
    {SKRMB_REG_TYPE_DISCRETE_INPUT,         0x0000, 1,                  &disinp},
    {SKRMB_REG_TYPE_HOLDING,                0x0000, sizeof(hold_reg),   hold_reg},
    {SKRMB_REG_TYPE_INPUT,                  0x0000, sizeof(input_reg),  input_reg},
};

static void skrmb_send_func_test(uint8_t *data, uint16_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        printf("%02X", data[i]);
    }
    printf("\r\n");
}

int main( int argc, char *arg[] )
{
    printf( "参数个数：%d\r\n", argc );
    printf( "指令：%s\r\n", arg[0] );
    for ( uint8_t i = 1; i < argc; i++ ) {
        printf( "参数：%s\r\n", arg[i] );
    }
    skrmb_dev_slave_create(1, 1, SKRMB_DEFAULT_BROADCAST_ADDR, modbus_slave_reg, sizeof(modbus_slave_reg)/sizeof(modbus_slave_reg[0]));
    skrmb_dev_master_create(2);
    skrmb_dev_add_port(1, 0, skrmb_send_func_test);
    skrmb_dev_add_port(2, 0, skrmb_send_func_test);
    // slave test
    skrmb_data_push(1, 0, read_coil_buf, sizeof(read_coil_buf));
    skrmb_dev_run(1);

    skrmb_data_push(1, 0, write_coil_single, sizeof(write_coil_single));
    skrmb_dev_run(1);
    printf("%d-%02X-%02X\r\n", coil_reg, coil_reg_m[0], coil_reg_m[1]);

    skrmb_data_push(1, 0, write_coil_mul, sizeof(write_coil_mul));
    skrmb_dev_run(1);
    printf("%d-%02X-%02X\r\n", coil_reg, coil_reg_m[0], coil_reg_m[1]);

    skrmb_data_push(1, 0, read_dis_input, sizeof(read_dis_input));
    skrmb_dev_run(1);

    skrmb_data_push(1, 0, read_hold, sizeof(read_hold));
    skrmb_dev_run(1);

    skrmb_data_push(1, 0, read_input, sizeof(read_input));
    skrmb_dev_run(1);

    Printf_array(hold_reg, sizeof(hold_reg));

    skrmb_data_push(1, 0, write_hold_single, sizeof(write_hold_single));
    skrmb_dev_run(1);
    Printf_array(hold_reg, sizeof(hold_reg));

    skrmb_data_push(1, 0, write_hold_mul, sizeof(write_hold_mul));
    skrmb_dev_run(1);
    Printf_array(hold_reg, sizeof(hold_reg));

    // master test
    skrmb_m_coil_request_read(2, 0, 1, 0, 4);
    skrmb_m_disinp_request_read(2, 0, 1, 1, 3);
    skrmb_m_hold_request_read(2, 0, 1, 2, 2);
    skrmb_m_input_request_read(2, 0, 1, 3, 1);

    skrmb_m_coil_request_single_write(2, 0, 1, 0, true);
    skrmb_m_coil_request_single_write(2, 0, 1, 0, false);
    skrmb_m_coil_request_multiple_write(2, 0, 1, 0, coil_w, 9);
    skrmb_m_hold_request_single_write(2, 0, 1, 0, 0xAABB);
    skrmb_m_hold_request_multiple_write(2, 0, 1, 0, (uint8_t *)hold_w, sizeof(hold_w));

    return 0;
}

































