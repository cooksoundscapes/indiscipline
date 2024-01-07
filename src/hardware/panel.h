#pragma once
#include "pcf-8574.h"
#include "gpio-base.h"
#include "panel-base.h"
#include <unordered_map>
#include <memory>

class Panel : public PanelBase {
	template<typename T>
	using I2CDevices = std::unordered_map<std::string, std::shared_ptr<T>>;

	I2CDevices<PCF8574> inputDevices;
  I2CDevices<PCF8574> outputDevices;

  std::shared_ptr<GPIOBase> gpio;
	
	std::unordered_map<std::string, ReadCB> callbacks;
	std::unique_ptr<ReadCB> currentCallback;

public:
  Panel(std::shared_ptr<GPIOBase>);

	void registerCallback(std::string name, ReadCB cb) override { callbacks.insert({name, cb}); }

	void setCurrentCallback(std::string cb) override;

  void setDeviceState(std::string device, std::bitset<8> state) override {
    if (outputDevices.find(device) == outputDevices.end())
      return;
    outputDevices[device]->write(state);
  }
};
