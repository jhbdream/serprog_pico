#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#include "bsp/board.h"
#include "tusb.h"

#include "serprog.h"
#include "led.h"
#include "serial.h"
#include "spi_dma.h"

static void cdc_task(void);

int main()
{
    stdio_uart_init();
    led_blinking_init();
    serprog_usb_init();
    board_spi_init();
    spi_flash_rdid();

    printf("board init ok!\n");

    while (1)
    {
        cdc_task();
        led_blinking_task();
    }
}

// echo to either Serial0 or Serial1
// with Serial0 as all lower case, Serial1 as all upper case
static void echo_serial_port(uint8_t itf, uint8_t buf[], uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        tud_cdc_n_write_char(itf, buf[i]);
    }
    tud_cdc_n_write_flush(itf);
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
static void cdc_task(void)
{
    static uint8_t itf = 0;
    static int cmd;

    if (tud_cdc_n_connected(itf))
    {
        if (tud_cdc_n_available(itf))
        {
            cmd = tud_cdc_n_read_char(itf);
            handle_cmd(cmd);
        }
    }
}