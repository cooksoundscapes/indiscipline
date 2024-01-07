#pragma once
#include <functional>
#include <string>
#include <bitset>

class PanelBase {
public:
  using ReadCB = std::function<void(std::string, int, int)>;
  virtual void registerCallback(std::string name, ReadCB cb) = 0;
	virtual void setCurrentCallback(std::string cb) = 0;
  virtual void setDeviceState(std::string device, std::bitset<8> state) = 0;
};