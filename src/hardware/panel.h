#pragma once
#include "pcf-8574.h"
#include "gpio.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <lo/lo.h>

class Panel {
	template<typename T>
	using I2CDevices = std::unordered_map<std::string, std::shared_ptr<T>>;

	I2CDevices<PCF8574> inputDevices;
  std::shared_ptr<GPIO> gpio;
  
	lo_address osc_addr = lo_address_new(NULL, "7778");
  void oscSend(std::string, int);

public:
  Panel();
};
