/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Example of writing via DMA to the SPI interface and similarly reading it back via a loopback.

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/dma.h"

#include "spi_dma.h"

uint dma_tx, dma_rx;

void board_spi_init(void)
{

    // max freq 62500000 HZ 31250000hz
    // Enable SPI at 1 MHz and connect to GPIOs
    spi_init(SPI_PORT, 31250000);

    gpio_set_function(PIN_CLK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_TX, GPIO_FUNC_SPI);
    gpio_set_function(PIN_RX, GPIO_FUNC_SPI);

    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(PIN_CLK, PIN_TX, PIN_RX, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(PIN_CS, "SPI CS"));

    // Grab some unused dma channels
    dma_tx = dma_claim_unused_channel(true);
    dma_rx = dma_claim_unused_channel(true);
}

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static void spi_dma_write_config(const uint8_t *buffer, uint32_t len, bool increase)
{
    // We set the inbound DMA to transfer from the SPI receive FIFO to a memory buffer paced by the SPI RX FIFO DREQ
    // We coinfigure the read address to remain unchanged for each element, but the write
    // address to increment (so data is written throughout the buffer)
    dma_channel_config c = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_index(spi_default) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
    channel_config_set_read_increment(&c, increase);
    channel_config_set_write_increment(&c, false);
    dma_channel_configure(dma_tx, &c,
                          &spi_get_hw(spi_default)->dr, // write address
                          buffer, // read address
                          len, // element count (each element is of size transfer_data_size)
                          false); // don't start yet
}

static void spi_dma_read_config(uint8_t *buffer, uint32_t len, bool increase)
{
    // We set the inbound DMA to transfer from the SPI receive FIFO to a memory buffer paced by the SPI RX FIFO DREQ
    // We coinfigure the read address to remain unchanged for each element, but the write
    // address to increment (so data is written throughout the buffer)
    dma_channel_config c = dma_channel_get_default_config(dma_rx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_index(spi_default) ? DREQ_SPI1_RX : DREQ_SPI0_RX);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, increase);

    dma_channel_configure(dma_rx, &c,
                          buffer, // write address
                          &spi_get_hw(spi_default)->dr, // read address
                          len, // element count (each element is of size transfer_data_size)
                          false); // don't start yet
}

static void spi_dma_transfer_enable()
{
    // start them exactly simultaneously to avoid races (in extreme cases the FIFO could overflow)
    dma_start_channel_mask((1u << dma_tx) | (1u << dma_rx));
    dma_channel_wait_for_finish_blocking(dma_rx);
    if (dma_channel_is_busy(dma_tx)) {
        panic("RX completed before TX");
    }
}

void spi_dma_write(char *buf, uint32_t len)
{
    static uint8_t tmp = 0;

    spi_dma_write_config(buf, len, true);
    spi_dma_read_config(&tmp, len, false);
    spi_dma_transfer_enable();
}

void spi_dma_read(char *buf, uint32_t len)
{
    static uint8_t tmp = 0;

    spi_dma_write_config(&tmp, len, false);
    spi_dma_read_config(buf, len, true);
    spi_dma_transfer_enable();
}

void spi_cs_select()
{
    cs_select(PIN_CS);
}

void spi_cs_deselect()
{
    cs_deselect(PIN_CS);
}

uint32_t spi_freq_config(uint32_t freq)
{
    return spi_set_baudrate(SPI_PORT, freq);
}

void spi_flash_rdid()
{
    char tx_buf[1] = {0x9f};
    char rx_buf[3];

    spi_cs_select();
    spi_dma_write(tx_buf, 1);
    spi_dma_read(rx_buf, 3);
    spi_cs_deselect();

    printf("rdid: %x %x %x\n",rx_buf[0], rx_buf[1], rx_buf[2]);
}