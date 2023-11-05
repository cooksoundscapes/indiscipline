#pragma once
#include "pcf-8574.h"
#include "panel-abstract.h"
#include "gpio.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

class Panel : public AbstractPanel {
  std::unordered_map<std::string, std::shared_ptr<PCF8574>> inputDevices;
  std::shared_ptr<GPIO> gpio;
  
  std::function<void(std::string, int)> buttonAction;
  std::function<void(std::string, int)> readDevice;

public:
  Panel() {
		buttonAction = [this](std::string name, int level){
			std::cout << "Button " << name << " is at " << level << '\n';
		};
		readDevice = [this](std::string device, int level){
			if (level == 0)
				read(device);
		};
		
		addDevice("encoders", std::make_shared<EncoderGroup>(0x20));
		
    this->gpio = std::make_shared<GPIO>();
    gpio->addController(23, {"system", buttonAction});
    gpio->addController(24, {"left", buttonAction});
    gpio->addController(25, {"right", buttonAction});
    gpio->addController(18, {"encoders", readDevice});
  }

  void addDevice(std::string name, std::shared_ptr<PCF8574> device) {
    inputDevices.insert({name, device});
  }

  void read() override {
    for (auto dev : inputDevices) {
      dev.second->read();
    }
  }
  void read(std::string name) {
    inputDevices[name]->read();
  }
};
