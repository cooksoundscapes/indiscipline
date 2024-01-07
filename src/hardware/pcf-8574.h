#pragma once
#include "i2c-device.h"
#include <bitset>
#include <vector>

class PCF8574 : public DeviceI2C
{
protected:
	std::bitset<8> raw_state;
	std::vector<int> state;

	void read_raw_state() {
		auto response = i2c_smbus_read_byte_data(file, 0xff);
		if (response < 0) {
			//std::cerr << "Error reading device" << '\n';
			return;
		}
		auto new_state = std::bitset<8>(response);
		raw_state = new_state;
	}

public:
	PCF8574(int addr, size_t s): DeviceI2C(addr) { 
		state.resize(s);
		i2c_smbus_write_byte(file, 0xff);
	}
	
	~PCF8574() {}

	virtual const std::vector<int>& read() {return state;}

	void write(std::bitset<8> arr) {
		i2c_smbus_write_byte(file, arr.to_ullong());
	}
};

class EncoderGroup : public PCF8574 {
	//gray code
  const std::array<std::bitset<4>, 4> cw = {0b1110, 0b1000, 0b0001, 0b0111};
  const std::array<std::bitset<4>, 4> ccw = {0b0100, 0b0010, 0b1011, 0b1101};
 
  std::array<int, 4> readEncoderData(const std::bitset<8>&);
  int getRotationDirection(int, int);

public:
  EncoderGroup(int addr) : PCF8574(addr, 4) {}
  ~EncoderGroup() {}

  const std::vector<int>& read() override;
}; 

class ButtonArray : public PCF8574 {
public:
	ButtonArray(int addr) : PCF8574(addr, 8) {}

	const std::vector<int>& read() override;
};

class LightArray : public PCF8574 {
public:
	LightArray(int addr) : PCF8574(addr, 8) {}
};
