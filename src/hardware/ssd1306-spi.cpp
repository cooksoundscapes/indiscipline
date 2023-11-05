#include "ssd1306-spi.h"
#include <iostream>
#include <fcntl.h> //open
#include <unistd.h>  //close (é mole?)

SSD1306_SPI::SSD1306_SPI() {
  spi_fd = open("/dev/spidev0.0", O_RDWR);
  if (spi_fd < 0) {
    std::cerr << "Failed to open SPI device." << std::endl;
    exit(1);
  }
  send_command(SSD1306_DISPLAYOFF);
	send_command(SSD1306_SETDISPLAYCLOCKDIV);
	send_command(0x80);
	send_command(SSD1306_SETMULTIPLEX);
	send_command(0x3F);
	send_command(SSD1306_SETDISPLAYOFFSET);
	send_command(0x00);
	send_command(SSD1306_SETSTARTLINE);
	send_command(SSD1306_CHARGEPUMP);
	send_command(0x14);
	send_command(SSD1306_MEMORYMODE);
	send_command(0x00);
	send_command(SSD1306_SEGMENT_REMAP);
	send_command(0xC8);
	send_command(SSD1306_SETCOMPINS);
	send_command(0x12);
	send_command(SSD1306_SETCONTRAST);
	send_command(0x80);
	send_command(SSD1306_SETPRECHARGE);
	send_command(SSD1306_PRE_CHARGE_PERIOD);
	send_command(0xA4);
	send_command(SSD1306_NORMALDISPLAY);
	send_command(SSD1306_DEACTIVATE_SCROLL);
	send_command(SSD1306_DISPLAYON);
}

SSD1306_SPI::~SSD1306_SPI() {
  close(spi_fd);
}

void SSD1306_SPI::send_command(uint8_t c) {
  write(spi_fd, &c, 1);
}