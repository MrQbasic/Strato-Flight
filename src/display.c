#include "display.h"

#include "driver/gpio.h"


bool display_init() {
    spi_bus_config_t buscfg = {
        .sclk_io_num = DISPLAY_PIN_SCLK,
        .mosi_io_num = DISPLAY_PIN_SDIN,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 320 * 240 * 2 + 8, // Set a buffer size!
    };
    spi_bus_initialize(DISPLAY_SPI_BUS, &buscfg, SPI_DMA_CH_AUTO);





    return false;
}