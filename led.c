#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "led.h"

#ifndef LED_PIN
#define LED_PIN PICO_DEFAULT_LED_PIN
#endif

static uint32_t blink_interval_ms = 1000;

void led_blinking_init(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, 1);
    gpio_put(LED_PIN, 0);
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
    static uint32_t start_ms = 0;
    static bool led_state = false;
    // Blink every interval ms
    if (board_millis() - start_ms < blink_interval_ms) return; // not enough time
    start_ms += blink_interval_ms;
    board_led_write(led_state);
    led_state = 1 - led_state; // toggle
}