#pragma once
#include "i2c-device.h"
#include <bitset>

class PCF8574 : public DeviceI2C {
protected:
  std::bitset<8> state;

public:
  PCF8574(int addr): DeviceI2C(addr) {}
	~PCF8574() {}

  void read() override;
};

class EncoderGroup : public PCF8574 {
	//gray code
  const std::array<std::bitset<4>, 4> cw = {0b1110, 0b1000, 0b0001, 0b0111};
  const std::array<std::bitset<4>, 4> ccw = {0b0100, 0b0010, 0b1011, 0b1101};

  std::array<int, 4> readEncoderData(const std::bitset<8>&);
  int getRotationDirection(int, int);

public:
  EncoderGroup(int addr) : PCF8574(addr) {}
  ~EncoderGroup() {}

  void read() override;
}; 
