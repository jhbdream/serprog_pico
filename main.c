#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#include "bsp/board.h"
#include "tusb.h"

static void cdc_task(void);

int main() {

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
  for(uint32_t i=0; i<count; i++)
  {
    if (itf == 0)
    {
      // echo back 1st port as lower case
      if (isupper(buf[i])) buf[i] += 'a' - 'A';
    }
    else
    {
      // echo back additional ports as upper case
      if (islower(buf[i])) buf[i] -= 'a' - 'A';
    }

    tud_cdc_n_write_char(itf, buf[i]);

    if ( buf[i] == '\r' ) tud_cdc_n_write_char(itf, '\n');
  }
  tud_cdc_n_write_flush(itf);
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
static void cdc_task(void)
{
    uint8_t itf = 0;


    if ( tud_cdc_n_connected(itf) )
    {
        if ( tud_cdc_n_available(itf) )
        {
            //echo_serial_port(1, "recv", 5);

        //uint8_t buf[64];

        //uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));
        
        // echo back to both serial ports
        //echo_serial_port(0, buf, count);
        //echo_serial_port(1, buf, count);
          uint8_t cmd;
          uint32_t count;
          count = tud_cdc_n_read(itf, &cmd, sizeof(cmd));
          echo_serial_port(1, &cmd, count);
          handle_cmd(cmd);
        }
    }
  
}
