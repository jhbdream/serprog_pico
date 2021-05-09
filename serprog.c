#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pico/stdlib.h"
#include "serprog.h"
#include "serial.h"

static int sp_doanswer(uint8_t answer, uint8_t cmd, uint32_t parmlen,
			uint8_t *params, uint32_t retlen, void *retparms)
{
	unsigned char c;

    /* 发送 ack/ noack */
    serialport_write(&answer, 1);

    /* 读取命令携带的内容 */
    serialport_read(params, parmlen);

    /* 返回内容 */
    serialport_write(retparms, retlen);

	return 0;
}

static int sp_doread(uint8_t answer, uint32_t parmlen,
			uint8_t *params, uint32_t retlen, void *retparms)
{

}

static int sp_dowrite(uint8_t answer, uint32_t parmlen,
			uint8_t *params, uint32_t retlen, void *retparms)
{

}

static uint16_t serprog_version = SERPROG_VERSION;
static uint32_t serprog_map[8] = {SERPROG_CMD_MAP};
static uint8_t serprog_name[16] = "pico serprog";
static uint16_t serprog_buffer_size = (uint16_t)SERPROG_BUFFER_SIZE;
static uint8_t serprog_bus_type = (uint8_t)SERPROG_BUS_TYPE;
static uint16_t sp_device_opbuf_size = 300;
/* The maximum length of an write_n operation; 0 = write-n not supported */
static uint32_t sp_max_write_n = 0;
static uint8_t serprog_max_write_n[3] = 
{
    ((SERPROG_MAX_WRITE_N >> 0) & 0xff),
    ((SERPROG_MAX_WRITE_N >> 8) & 0xff),
    ((SERPROG_MAX_WRITE_N >> 16) & 0xff),

};

static uint8_t ack = (uint8_t)S_ACK;
static uint8_t no_ack = (uint8_t)S_NAK;


serprog_answer_t serprog_handle[S_CMD_NUM] = 
{
    [S_CMD_NOP]         = ANSWER_DECLARE(S_ACK, NULL, 0, NULL, 0, sp_doanswer, NULL, NULL),

    [S_CMD_Q_IFACE]     = ANSWER_DECLARE(S_ACK, NULL, 0, &serprog_version,
                                sizeof(serprog_version), sp_doanswer, NULL, NULL),

    [S_CMD_Q_CMDMAP]    = ANSWER_DECLARE(S_ACK, NULL, 0, &serprog_map,
                                sizeof(serprog_map), sp_doanswer, NULL, NULL),

    [S_CMD_Q_PGMNAME]   = ANSWER_DECLARE(S_ACK, NULL, 0, serprog_name,
                                sizeof(serprog_name), sp_doanswer, NULL, NULL),

    [S_CMD_Q_SERBUF]    = ANSWER_DECLARE(S_ACK, NULL, 0, &serprog_buffer_size,
                                sizeof(serprog_buffer_size), sp_doanswer, NULL, NULL),

    [S_CMD_Q_BUSTYPE]   = ANSWER_DECLARE(S_ACK, NULL, 0, &serprog_bus_type,
                                sizeof(serprog_bus_type), sp_doanswer, NULL, NULL),

    [S_CMD_Q_CHIPSIZE]  = ANSWER_DECLARE(S_ACK, NULL, 0, NULL, 0, sp_doanswer, NULL, NULL),

    [S_CMD_Q_OPBUF]     = ANSWER_DECLARE(S_ACK, NULL, 0, &sp_device_opbuf_size,
                                sizeof(sp_device_opbuf_size), sp_doanswer, NULL, NULL),

    [S_CMD_Q_WRNMAXLEN] = ANSWER_DECLARE(S_ACK, NULL, 0, serprog_max_write_n,
                                sizeof(serprog_max_write_n), sp_doanswer, NULL, NULL),

    [S_CMD_R_BYTE]      = ANSWER_DECLARE(S_ACK, NULL, 3, NULL, 1, sp_doanswer, NULL, NULL),
    [S_CMD_R_NBYTES]    = ANSWER_DECLARE(S_ACK, NULL, 6, NULL, 0, sp_doanswer, NULL, NULL),
    [S_CMD_O_INIT]      = ANSWER_DECLARE(S_ACK, NULL, 0, NULL, 0, sp_doanswer, NULL, NULL),
    [S_CMD_O_WRITEB]    = ANSWER_DECLARE(S_ACK, NULL, 4, NULL, 0, sp_doanswer, NULL, NULL),
    [S_CMD_O_WRITEN]    = ANSWER_DECLARE(S_ACK, NULL, 6, NULL, 0, sp_doanswer, NULL, NULL),
    [S_CMD_O_DELAY]     = ANSWER_DECLARE(S_ACK, NULL, 0, NULL, 0, sp_doanswer, NULL, NULL),
    [S_CMD_O_EXEC]      = ANSWER_DECLARE(S_ACK, NULL, 0, NULL, 0, sp_doanswer, NULL, NULL),
    [S_CMD_SYNCNOP]     = ANSWER_DECLARE(S_NAK, NULL, 0, &ack, sizeof(ack), sp_doanswer, NULL, NULL),
    [S_CMD_Q_RDNMAXLEN] = ANSWER_DECLARE(S_ACK, NULL, 0, NULL, 0, sp_doanswer, NULL, NULL),
    [S_CMD_S_BUSTYPE]   = ANSWER_DECLARE(S_ACK, NULL, 0, NULL, 0, sp_doanswer, NULL, NULL),
    [S_CMD_O_SPIOP]     = ANSWER_DECLARE(S_ACK, NULL, 6, NULL, 0, sp_doanswer, NULL, NULL),
    [S_CMD_S_SPI_FREQ]  = ANSWER_DECLARE(S_ACK, NULL, 0, NULL, 0, NULL, NULL, NULL),
    [S_CMD_S_PIN_STATE] = ANSWER_DECLARE(S_ACK, NULL, 0, NULL, 0, NULL, NULL, NULL),
};

int handle_cmd(uint8_t cmd)
{
    static serprog_answer_t *process;
 
    static uint8_t ack;
    static void *params;
    static uint32_t parmlen;

    static void *retparms;
    static uint32_t retlen;

    process = &serprog_handle[cmd];
    ack = process->ack;
    params = process->params;
    parmlen = process->parmlen;

    retparms = process->retparms;
    retlen = process->retlen;

    process->answer(ack, cmd, parmlen, params, retlen, retparms);
}