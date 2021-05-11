#ifndef _SERIAL_USB_H
#define _SERIAL_USB_H

// PICO_CONFIG: PICO_SERIAL_USB_STDOUT_TIMEOUT_US, Number of microseconds to be blocked trying to write USB output before assuming the host has disappeared and discarding data, default=500000, group=pico_SERIAL_usb
#ifndef PICO_SERIAL_USB_STDOUT_TIMEOUT_US
#define PICO_SERIAL_USB_STDOUT_TIMEOUT_US 500000
#endif

// todo perhaps unnecessarily frequent?
// PICO_CONFIG: PICO_SERIAL_USB_TASK_INTERVAL_US, Period of microseconds between calling tud_task in the background, default=1000, advanced=true, group=pico_SERIAL_usb
#ifndef PICO_SERIAL_USB_TASK_INTERVAL_US
#define PICO_SERIAL_USB_TASK_INTERVAL_US 1000
#endif

// PICO_CONFIG: PICO_SERIAL_USB_LOW_PRIORITY_IRQ, low priority (non hardware) IRQ number to claim for tud_task() background execution, default=31, advanced=true, group=pico_SERIAL_usb
#ifndef PICO_SERIAL_USB_LOW_PRIORITY_IRQ
#define PICO_SERIAL_USB_LOW_PRIORITY_IRQ 31
#endif

// PICO_CONFIG: PICO_SERIAL_USB_ENABLE_RESET_VIA_BAUD_RATE, Enable/disable resetting into BOOTSEL mode if the host sets the baud rate to a magic value (PICO_SERIAL_USB_RESET_MAGIC_BAUD_RATE), type=bool, default=1, group=pico_SERIAL_usb
#ifndef PICO_SERIAL_USB_ENABLE_RESET_VIA_BAUD_RATE
#define PICO_SERIAL_USB_ENABLE_RESET_VIA_BAUD_RATE 1
#endif

// PICO_CONFIG: PICO_SERIAL_USB_RESET_MAGIC_BAUD_RATE, baud rate that if selected causes a reset into BOOTSEL mode (if PICO_SERIAL_USB_ENABLE_RESET_VIA_BAUD_RATE is set), default=1200, group=pico_SERIAL_usb
#ifndef PICO_SERIAL_USB_RESET_MAGIC_BAUD_RATE
#define PICO_SERIAL_USB_RESET_MAGIC_BAUD_RATE 1200
#endif

// PICO_CONFIG: PICO_SERIAL_USB_RESET_BOOTSEL_ACTIVITY_LED, Optionally define a pin to use as bootloader activity LED when BOOTSEL mode is entered via USB (either VIA_BAUD_RATE or VIA_VENDOR_INTERFACE), type=int, min=0, max=29, group=pico_SERIAL_usb

// PICO_CONFIG: PICO_SERIAL_USB_RESET_BOOTSEL_FIXED_ACTIVITY_LED, Whether the pin specified by PICO_SERIAL_USB_RESET_BOOTSEL_ACTIVITY_LED is fixed or can be modified by picotool over the VENDOR USB interface, type=bool, default=0, group=pico_SERIAL_usb
#ifndef PICO_SERIAL_USB_RESET_BOOTSEL_FIXED_ACTIVITY_LED
#define PICO_SERIAL_USB_RESET_BOOTSEL_FIXED_ACTIVITY_LED 0
#endif

// Any modes disabled here can't be re-enabled by picotool via VENDOR_INTERFACE.
// PICO_CONFIG: PICO_SERIAL_USB_RESET_BOOTSEL_INTERFACE_DISABLE_MASK, Optionally disable either the mass storage interface (bit 0) or the PICOBOOT interface (bit 1) when entering BOOTSEL mode via USB (either VIA_BAUD_RATE or VIA_VENDOR_INTERFACE), type=int, min=0, max=3, default=0, group=pico_SERIAL_usb
#ifndef PICO_SERIAL_USB_RESET_BOOTSEL_INTERFACE_DISABLE_MASK
#define PICO_SERIAL_USB_RESET_BOOTSEL_INTERFACE_DISABLE_MASK 0u
#endif

// PICO_CONFIG: PICO_SERIAL_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE, Enable/disable resetting into BOOTSEL mode via an additional VENDOR USB interface - enables picotool based reset, type=bool, default=1, group=pico_SERIAL_usb
#ifndef PICO_SERIAL_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE
#define PICO_SERIAL_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE 1
#endif

// PICO_CONFIG: PICO_SERIAL_USB_RESET_INTERFACE_SUPPORT_RESET_TO_BOOTSEL, If vendor reset interface is included allow rebooting to BOOTSEL mode, type=bool, default=1, group=pico_SERIAL_usb
#ifndef PICO_SERIAL_USB_RESET_INTERFACE_SUPPORT_RESET_TO_BOOTSEL
#define PICO_SERIAL_USB_RESET_INTERFACE_SUPPORT_RESET_TO_BOOTSEL 1
#endif

// PICO_CONFIG: PICO_SERIAL_USB_RESET_INTERFACE_SUPPORT_RESET_TO_FLASH_BOOT, If vendor reset interface is included allow rebooting with regular flash boot, type=bool, default=1, group=pico_SERIAL_usb
#ifndef PICO_SERIAL_USB_RESET_INTERFACE_SUPPORT_RESET_TO_FLASH_BOOT
#define PICO_SERIAL_USB_RESET_INTERFACE_SUPPORT_RESET_TO_FLASH_BOOT 1
#endif

// PICO_CONFIG: PICO_SERIAL_USB_RESET_RESET_TO_FLASH_DELAY_MS, delays in ms before rebooting via regular flash boot, default=100, group=pico_SERIAL_usb
#ifndef PICO_SERIAL_USB_RESET_RESET_TO_FLASH_DELAY_MS
#define PICO_SERIAL_USB_RESET_RESET_TO_FLASH_DELAY_MS 100
#endif


#ifdef __cplusplus
extern "C" {
#endif

bool serprog_usb_init(void);
int serialport_write(const unsigned char *buf, unsigned int writecnt);
int serialport_read(unsigned char *buf, unsigned int readcnt);

#ifdef __cplusplus
}
#endif

#endif
