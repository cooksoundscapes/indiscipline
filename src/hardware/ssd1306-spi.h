#include "ssd1306-base.h"
#include "linux/spi/spidev.h" 

class SSD1306_SPI {
  int spi_fd;

  void send_command(uint8_t);

public:
  SSD1306_SPI();
  ~SSD1306_SPI();
};