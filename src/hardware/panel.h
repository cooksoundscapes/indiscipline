#pragma once
#include "pcf-8574.h"
#include "panel-abstract.h"
#include "gpio.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <lo/lo.h>

class Panel : public AbstractPanel {
	template<typename T>
	using I2CDevices = std::unordered_map<std::string, std::shared_ptr<T>>;

	I2CDevices<PCF8574> inputDevices;
  std::shared_ptr<GPIO> gpio;
  
	lo_address osc_addr = lo_address_new(NULL, "7778");
  void oscSend(std::string, int);

public:
  Panel();

  void read() override {
    for (auto dev : inputDevices) {
      dev.second->read();
    }
  }
  const std::vector<int>& read(std::string name) {
    inputDevices[name]->read();
  }
};
