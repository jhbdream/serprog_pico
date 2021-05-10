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

static void cdc_task(void);

int main()
{

    board_init();
    tusb_init();

    while (1)
    {
        tud_task(); // tinyusb device task
        cdc_task();
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

    if (tud_cdc_n_connected(itf))
    {
        if (tud_cdc_n_available(itf))
        {
            uint8_t cmd;
            cmd = tud_cdc_n_read_char(itf);
            echo_serial_port(1, &cmd, 1);
            handle_cmd(cmd);
        }
    }
}
