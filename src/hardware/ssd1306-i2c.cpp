#include "ssd1306-i2c.h"
#include <i2c/smbus.h>
#include <string.h>
#include <unistd.h>
#include <bitset>

SSD1306::SSD1306(int addr) : DeviceI2C(addr)
{
	width = 128;
	height = 64;

	sendCommand(0xAE); // Entire Display OFF
	sendCommand(0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
	sendCommand(0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
	sendCommand(0xA8); // Set Multiplex Ratio
	sendCommand(0x3F); // 64 COM lines
	sendCommand(0xD3); // Set display offset
	sendCommand(0x00); // 0 offset
	sendCommand(0x40); // Set first line as the start line of the display
	sendCommand(0x8D); // Charge pump
	sendCommand(0x14); // Enable charge dump during display on
	sendCommand(0x20); // Set memory addressing mode
	sendCommand(0x00); // Horizontal addressing mode
	sendCommand(0xA1); // Set segment remap with column address 127 mapped to segment 0
	sendCommand(0xC8); // Set com output scan direction, scan from com63 to com 0
	sendCommand(0xDA); // Set com pins hardware configuration
	sendCommand(0x12); // Alternative com pin configuration, disable com left/right remap
	sendCommand(0x81); // Set contrast control
	sendCommand(0x80); // Set Contrast to 128
	sendCommand(0xD9); // Set pre-charge period
	sendCommand(0xF1); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
	sendCommand(0xA4); // Entire display ON, resume to RAM content display
	sendCommand(0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
	sendCommand(0x2E); // Deactivate scroll
	sendCommand(0xAF); // Display ON in normal mode

	clear();
}

void SSD1306::sendCommand(uint8_t command)
{	
	i2c_smbus_write_byte_data(file, 0x00, command);
}

void SSD1306::sendData(DisplayData data) {
	for (int i{0}; i < B_SIZE; i++) {
		i2c_smbus_write_byte_data(file, 0x40, data[i]);
	}
}

void SSD1306::clear(int color)
{
	int hex = (color > 0) ? 0xFF : 0x00;
	DisplayData buffer;
	for (auto &p : buffer) {
		p = hex;
	}
	sendData(buffer);
}

void SSD1306::stripes() {
	DisplayData buff;
	for (int i{0}; i < B_SIZE; i++) {
		if (((i / 128) % 2) == 1) {
			buff[i] = 0xFF;
		} else {
			buff[i] = 0x00;
		}
	}
	sendData(buff);
}

void SSD1306::switchOnOff() {
	if (isOn == 1) {
		sendCommand(SSD1306_DISPLAYOFF);
		isOn = 0;
	} else {
		sendCommand(SSD1306_DISPLAYON);
		isOn = 1;
	}
}

void SSD1306::rotateDisplay(uint8_t Rotate)
{
	if(Rotate > 1) Rotate = 1;

	this->sendCommand(0xA0 | (0x01 & Rotate));  // Set Segment Re-Map Default
							// 0xA0 (0x00) => column Address 0 mapped to 127
                			// 0xA1 (0x01) => Column Address 127 mapped to 0

	this->sendCommand(0xC0 | (0x08 & (Rotate<<3)));  // Set COM Output Scan Direction
							// 0xC0	(0x00) => normal mode (RESET) Scan from COM0 to COM[N-1];Where N is the Multiplex ratio.
							// 0xC8	(0xC8) => remapped mode. Scan from COM[N-1] to COM0;;Where N is the Multiplex ratio.
}

void SSD1306::drawBuffer(unsigned char* buffer) 
{
	/* IMPORTANT: RPi systems are Little-endian!
	 * A1 format gives 8 pixels per byte, but on raspberries they are inverted!!
	 * also, they are packed horizontally. what a pain.
	 */
	SSD1306::DisplayData displayData;
	
	for (int i{0}; i < B_SIZE; i++) {
		std::bitset<8> segment;
		int pageOffset = (i / 128) * 1024;
		int row = i % 128;
		for (int b{7}; b >= 0; b--) {
			int coord = (b * 128) + row + pageOffset;
			segment[b] = buffer[coord];
		}	
		displayData[i] = (uint8_t)segment.to_ulong();
	}

	sendData(displayData);
}