#include "ssd1306-base.h"
#include "ssd1306-lib.h"
#include "linux/spi/spidev.h" 

class SSD1306_SPI : public SSD1306Base {
  int spi_fd;

  void send_command(uint8_t);

public:
  SSD1306_SPI();
  ~SSD1306_SPI();

  void update(std::vector<unsigned char>) override;
};