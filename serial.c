#include "tusb.h"

#include "pico/time.h"
#include "pico/binary_info.h"
#include "pico/mutex.h"
#include "hardware/irq.h"

#include "serial.h"

static mutex_t serial_usb_mutex;

static void low_priority_worker_irq(void)
{
    // if the mutex is already owned, then we are in user code
    // in this file which will do a tud_task itself, so we'll just do nothing
    // until the next tick; we won't starve
    if (mutex_try_enter(&serial_usb_mutex, NULL))
    {
        tud_task();
        mutex_exit(&serial_usb_mutex);
    }
}

static int64_t timer_task(__unused alarm_id_t id, __unused void *user_data)
{
    irq_set_pending(PICO_SERIAL_USB_LOW_PRIORITY_IRQ);
    return PICO_SERIAL_USB_TASK_INTERVAL_US;
}

bool serprog_usb_init(void)
{
    // initialize TinyUSB
    tusb_init();

    irq_set_exclusive_handler(PICO_SERIAL_USB_LOW_PRIORITY_IRQ, low_priority_worker_irq);
    irq_set_enabled(PICO_SERIAL_USB_LOW_PRIORITY_IRQ, true);

    mutex_init(&serial_usb_mutex);
    bool rc = add_alarm_in_us(PICO_SERIAL_USB_TASK_INTERVAL_US, timer_task, NULL, true);

    return rc;
}



static void usb_cdc_write(const char *buf, int length) {
    static uint64_t last_avail_time;
    uint32_t owner;
    if (!mutex_try_enter(&serial_usb_mutex, &owner)) {
        if (owner == get_core_num()) return; // would deadlock otherwise
        mutex_enter_blocking(&serial_usb_mutex);
    }
    if (tud_cdc_connected()) {
        for (int i = 0; i < length;) {
            int n = length - i;
            int avail = tud_cdc_write_available();
            if (n > avail) n = avail;
            if (n) {
                int n2 = tud_cdc_write(buf + i, n);
                tud_task();
                tud_cdc_write_flush();
                i += n2;
                last_avail_time = time_us_64();
            } else {
                tud_task();
                tud_cdc_write_flush();
                if (!tud_cdc_connected() ||
                    (!tud_cdc_write_available() && time_us_64() > last_avail_time + PICO_SERIAL_USB_STDOUT_TIMEOUT_US)) {
                    break;
                }
            }
        }
    } else {
        // reset our timeout
        last_avail_time = 0;
    }
    mutex_exit(&serial_usb_mutex);
}


static int usb_cdc_read(char *buf, int length) {
    uint32_t owner;
    if (!mutex_try_enter(&serial_usb_mutex, &owner)) {
        if (owner == get_core_num()) return PICO_ERROR_NO_DATA; // would deadlock otherwise
        mutex_enter_blocking(&serial_usb_mutex);
    }
    int rc = PICO_ERROR_NO_DATA;
    if (tud_cdc_connected() && tud_cdc_available()) {
        int count = tud_cdc_read(buf, length);
        rc =  count ? count : PICO_ERROR_NO_DATA;
    }
    mutex_exit(&serial_usb_mutex);
    return rc;
}

int serialport_write(const unsigned char *buf, unsigned int writecnt)
{
    usb_cdc_write(buf, writecnt);
}

int serialport_read(unsigned char *buf, unsigned int readcnt)
{
    int tmp = 0;
	while (readcnt > 0)
    {
		tmp = usb_cdc_read(buf, readcnt);
        if(tmp == PICO_ERROR_NO_DATA)
        {
            continue;
        }

        readcnt -= tmp;
		buf += tmp;
    }
}