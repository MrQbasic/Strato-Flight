#include "driver/spi_master.h"

#define DISPLAY_PIN_SDIN 42
#define DISPLAY_PIN_SCLK 41
#define DISPLAY_PIN_RS   40
#define DISPLAY_PIN_RES  39
#define DISPLAY_PIN_CS   41
#define DISPLAY_PIN_LED  21

#define DISPLAY_SPI_BUS SPI2_HOST

bool display_init();