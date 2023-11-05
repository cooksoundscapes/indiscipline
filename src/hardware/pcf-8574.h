#pragma once
#include "i2c-device.h"
#include <bitset>

template<int S>
class PCF8574 : public DeviceI2C
{
public:
	using State = std::array<int, S>;
	PCF8574(int addr): DeviceI2C(addr) {}
	~PCF8574() {}

	virtual const State& read() = 0;

protected:
  std::bitset<8> raw_state;

  void read_raw_state() {
		auto response = i2c_smbus_read_byte_data(file, 0xff);
		if (response < 0) {
			//std::cerr << "Error reading device" << '\n';
			return;
		}
		auto new_state = std::bitset<8>(response);
		raw_state = new_state;
	}
	
	State state;
};

class EncoderGroup : public PCF8574<4> {
	//gray code
  const std::array<std::bitset<4>, 4> cw = {0b1110, 0b1000, 0b0001, 0b0111};
  const std::array<std::bitset<4>, 4> ccw = {0b0100, 0b0010, 0b1011, 0b1101};
 
  State readEncoderData(const std::bitset<8>&);
  int getRotationDirection(int, int);

public:
  EncoderGroup(int addr) : PCF8574(addr) {}
  ~EncoderGroup() {}

  const State& read() override;
}; 
