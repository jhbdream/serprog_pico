#ifndef _SPI_DMA_H
#define _SPI_DMA_H

/* SPI pin define in CmakeLists.txt */
#define PIN_CLK 2
#define PIN_TX  3
#define PIN_RX  4
#define PIN_CS  5

#define SPI_PORT spi0

void board_spi_init(void);
void spi_dma_write(char *buf, uint32_t len);
void spi_dma_read(char *buf, uint32_t len);
void spi_cs_select();
void spi_cs_deselect();
uint32_t spi_freq_config(uint32_t freq);
void spi_flash_rdid();

#endif