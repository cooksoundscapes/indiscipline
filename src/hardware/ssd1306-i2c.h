#include "ssd1306-base.h"
#include "i2c-device.h"


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

