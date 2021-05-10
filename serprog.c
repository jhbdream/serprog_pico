#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pico/stdlib.h"
#include "serprog.h"
#include "serial.h"

static int process_answer(uint8_t cmd, uint8_t ack, uint32_t parmlen,
			uint8_t *params, uint32_t retlen, void *retparms);

static uint16_t serprog_version = SERPROG_VERSION;
static uint32_t serprog_map[8] = {SERPROG_CMD_MAP};
static uint8_t *sp_cmdmap = (uint8_t *)serprog_map;
static uint8_t serprog_name[16] = "pico serprog";
static uint16_t serprog_buffer_size = (uint16_t)SERPROG_BUFFER_SIZE;
static uint8_t serprog_bus_type = (uint8_t)SERPROG_BUS_TYPE;

static uint8_t ack = (uint8_t)S_ACK;
static uint8_t no_ack = (uint8_t)S_NAK;
static uint8_t serprog_set_bus_type;

static int sp_check_commandavail(uint8_t command)
{
	int byteoffs, bitoffs;
	byteoffs = command / 8;
	bitoffs = command % 8;
	return (sp_cmdmap[byteoffs] & (1 << bitoffs)) ? 1 : 0;
}

serprog_answer_t serprog_handle[S_CMD_NUM] =
{
    /* simple answer */
    [S_CMD_NOP]         = ACK_ANSWER_DECLARE(S_CMD_NOP, NULL, 0, NULL, 0, process_answer),

    [S_CMD_Q_IFACE]     = ACK_ANSWER_DECLARE(S_CMD_Q_IFACE, NULL, 0,
                            &serprog_version, sizeof(serprog_version), process_answer),

    [S_CMD_Q_CMDMAP]    = ACK_ANSWER_DECLARE(S_CMD_Q_CMDMAP, NULL, 0,
                            &serprog_map, sizeof(serprog_map), process_answer),

    [S_CMD_Q_PGMNAME]   = ACK_ANSWER_DECLARE(S_CMD_Q_PGMNAME, NULL, 0,
                            &serprog_name, sizeof(serprog_name), process_answer),

    [S_CMD_Q_SERBUF]    = ACK_ANSWER_DECLARE(S_CMD_Q_SERBUF, NULL, 0,
                            &serprog_buffer_size, sizeof(serprog_buffer_size), process_answer),

    [S_CMD_Q_BUSTYPE]   = ACK_ANSWER_DECLARE(S_CMD_Q_BUSTYPE, NULL, 0,
                            &serprog_bus_type, sizeof(serprog_bus_type), process_answer),

    [S_CMD_SYNCNOP]     = NAK_ANSWER_DECLARE(S_CMD_SYNCNOP, NULL, 0,
                            &ack, sizeof(ack), process_answer),

    /* 1字节参数 */
    [S_CMD_S_BUSTYPE]   = ACK_ANSWER_DECLARE(S_CMD_S_BUSTYPE, &serprog_set_bus_type, 1,
                            NULL, 0, process_answer),

    /* 3字节写长度 3字节读长度 3字节写地址 */
    [S_CMD_O_SPIOP]     = ACK_ANSWER_DECLARE(S_CMD_O_SPIOP, NULL, 6,
                            NULL, 0, process_answer),

    /* 4字节设置频率 4字节返回频率 */
    [S_CMD_S_SPI_FREQ]  = ACK_ANSWER_DECLARE(S_CMD_S_SPI_FREQ, NULL, 4,
                            NULL, 4, process_answer),
};

static int process_answer(uint8_t cmd, uint8_t ack, uint32_t parmlen,
			uint8_t *params, uint32_t retlen, void *retparms)
{

    /* 发送 ack/ noack */
    serialport_write(&ack, 1);

    /* 读取命令携带的内容 */
    serialport_read(params, parmlen);

    /* 返回内容 */
    serialport_write(retparms, retlen);

	return 0;
}

int handle_cmd(uint8_t cmd)
{
    static serprog_answer_t *process;

	if (sp_check_commandavail(cmd) != 1)
		return 1;

    process = &serprog_handle[cmd];

    process->answer(process->cmd,
                    process->ack,
                    process->parmlen,
                    process->params,
                    process->retlen,
                    process->retparms
    );
}