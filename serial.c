#include "serial.h"
#include "tusb.h"

int serialport_write(const unsigned char *buf, unsigned int writecnt)
{
    tud_cdc_n_write(0, buf, writecnt);
    tud_cdc_n_write(1, buf, writecnt);

    tud_cdc_n_write_flush(0);
    tud_cdc_n_write_flush(1);

    return 0;
}

int serialport_read(unsigned char *buf, unsigned int readcnt)
{
    return tud_cdc_n_read(0, buf, readcnt);
}