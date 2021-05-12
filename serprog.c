#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pico/stdlib.h"
#include "serprog.h"
#include "serial.h"
#include "spi_dma.h"

static int process_answer(uint8_t cmd, uint8_t ack, uint32_t parmlen,
			uint8_t *params, uint32_t retlen, void *retparms);

static int process_spi_operation(uint8_t cmd, uint8_t ack, uint32_t parmlen,
			uint8_t *params, uint32_t retlen, void *retparms);

static int spi_freq_set(uint8_t cmd, uint8_t ack, uint32_t parmlen,
			uint8_t *params, uint32_t retlen, void *retparms);

static uint16_t serprog_version = SERPROG_VERSION;

static uint32_t serprog_map[8] = { SERPROG_CMD_MAP };
static uint8_t *sp_cmdmap = (uint8_t *)serprog_map;

static uint8_t serprog_name[16] = SERPROG_NAME;
static uint16_t serprog_buffer_size = (uint16_t)SERPROG_BUFFER_SIZE;
static uint8_t serprog_bus_type = (uint8_t)SERPROG_BUS_TYPE;


static uint8_t ack = (uint8_t)S_ACK;
static uint8_t no_ack = (uint8_t)S_NAK;
static uint8_t params_buf[16];

static uint32_t spi_freq;

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
    [S_CMD_NOP]         = ACK_ANSWER_DECLARE(S_CMD_NOP, 0, NULL, 0, NULL, process_answer),

    [S_CMD_Q_IFACE]     = ACK_ANSWER_DECLARE(S_CMD_Q_IFACE, 0, NULL,
                            sizeof(serprog_version), &serprog_version, process_answer),

    [S_CMD_Q_CMDMAP]    = ACK_ANSWER_DECLARE(S_CMD_Q_CMDMAP, 0, NULL,
                            sizeof(serprog_map), &serprog_map, process_answer),

    [S_CMD_Q_PGMNAME]   = ACK_ANSWER_DECLARE(S_CMD_Q_PGMNAME, 0, NULL,
                            sizeof(serprog_name), &serprog_name, process_answer),

    [S_CMD_Q_SERBUF]    = ACK_ANSWER_DECLARE(S_CMD_Q_SERBUF, 0, NULL,
                            sizeof(serprog_buffer_size), &serprog_buffer_size, process_answer),

    [S_CMD_Q_BUSTYPE]   = ACK_ANSWER_DECLARE(S_CMD_Q_BUSTYPE, 0, NULL,
                            sizeof(serprog_bus_type), &serprog_bus_type, process_answer),

    [S_CMD_SYNCNOP]     = NAK_ANSWER_DECLARE(S_CMD_SYNCNOP, 0, NULL,
                            sizeof(ack), &ack, process_answer),

    /* 1字节参数 */
    [S_CMD_S_BUSTYPE]   = ACK_ANSWER_DECLARE(S_CMD_S_BUSTYPE, 1, params_buf,
                            0, NULL, process_answer),

    /* 3字节写长度 3字节读长度 3字节写地址 */
    [S_CMD_O_SPIOP]     = ACK_ANSWER_DECLARE(S_CMD_O_SPIOP, 6, params_buf,
                            0, NULL, process_answer),

    /* 4字节设置频率 4字节返回频率 */
    [S_CMD_S_SPI_FREQ]  = ACK_ANSWER_DECLARE(S_CMD_S_SPI_FREQ, 4, params_buf,
                            4, params_buf, spi_freq_set),
};

/**
 * @brief 处理普通命令
 *          PC: COMMAND
 *          DEVICE: ACK/NAK + PARAMTER
 *
 * @param cmd
 * @param ack
 * @param parmlen
 * @param params
 * @param retlen
 * @param retparms
 * @return int
 */
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

/**
 * @brief 处理数据传输命令
 *          PC: COMMAND + 3byte writecnt 3byte readcnt + nbyte write_date
 *          DEVICE: ACK + rlen bytes of data / NAK
 *
 * @param cmd
 * @param ack
 * @param parmlen
 * @param params
 * @param retlen
 * @param retparms
 * @return int
 */
static int process_spi_operation(uint8_t cmd, uint8_t ack, uint32_t parmlen,
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

static int spi_freq_set(uint8_t cmd, uint8_t ack, uint32_t parmlen,
			uint8_t *params, uint32_t retlen, void *retparms)
{
    uint32_t f_spi_req, f_spi;
    uint8_t *buf = (uint8_t *)retparms;

    /* 发送 ack/ noack */
    serialport_write(&ack, 1);

    /* 读取命令携带的内容 */
    serialport_read(params, parmlen);

    f_spi_req = params[0];
    f_spi_req |= params[1] << (1 * 8);
    f_spi_req |= params[2] << (2 * 8);
    f_spi_req |= params[3] << (3 * 8);

    f_spi = spi_freq_config(f_spi_req);

    buf[0] = (f_spi >> (0 * 8)) & 0xFF;
    buf[1] = (f_spi >> (1 * 8)) & 0xFF;
    buf[2] = (f_spi >> (2 * 8)) & 0xFF;
    buf[3] = (f_spi >> (3 * 8)) & 0xFF;

    /* 返回内容 */
    serialport_write(buf, retlen);

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