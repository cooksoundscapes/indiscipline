#pragma once
#include "i2c_device.h"
#include <cstdint>

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

#define B_SIZE 1024

class SSD1306 : public DeviceI2C {
public:
	int width, height, isOn;

	using DisplayData = std::array<uint8_t, B_SIZE>;

	SSD1306(int addr);
	
	void sendCommand(uint8_t command);
	void sendData(DisplayData data);

	void invertColors(uint8_t Invert);
	void rotateDisplay(uint8_t Rotate);
	void setContrast(uint8_t Contrast);

	void clear(int color = 0);
	void switchOnOff();
	void drawBuffer(unsigned char* buffer);

	// testing  functions:
	void stripes();
	void blockFill();
};
