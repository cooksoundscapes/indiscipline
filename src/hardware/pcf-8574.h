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
  const std::array<std::bitset<4>, 4> cw = {0xE, 0x8, 0x1, 0x7};
  const std::array<std::bitset<4>, 4> ccw = {0x4, 0x2, 0xB, 0xD};

  std::array<int, 4> readEncoderData(const std::bitset<8>&);
  int getRotationDirection(int, int);

public:
  EncoderGroup(int addr) : PCF8574(addr) {}
  ~EncoderGroup() {}

  void read() override;
}; 