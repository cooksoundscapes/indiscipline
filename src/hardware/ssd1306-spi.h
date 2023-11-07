#include "ssd1306-base.h"
#include "ssd1306-lib.h"
#include "gpio-base.h"
#include <linux/spi/spidev.h>
#include <memory>
#include <array>

class SSD1306_SPI : public SSD1306Base {
  int spi_fd;

  void send_command(uint8_t);

  std::shared_ptr<GPIOBase> gpio;

  std::array<uint8_t, B_SIZE> buffer;
  void sortPixels(std::vector<uint8_t>&);

  const int RESETPIN = 13;
  const int DCPIN = 6;

public:
  SSD1306_SPI(std::shared_ptr<GPIOBase> gpio);
  ~SSD1306_SPI();

  void update(std::vector<uint8_t>&) override;
};