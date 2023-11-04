#pragma once
#include "pcf-8574.h"
#include "panel-abstract.h"
#include "gpio.h"
#include <vector>
#include <memory>

class Panel : public AbstractPanel {
  std::vector<std::shared_ptr<PCF8574>> inputDevices;
  std::shared_ptr<GPIO> gpio;

public:
  Panel() {
    this->gpio = std::make_shared<GPIO>();
    gpio->registerPin("system", 23, PI_INPUT);
    gpio->registerPin("left", 24, PI_INPUT);
    gpio->registerPin("right", 25, PI_INPUT);
    addDevice(std::make_shared<EncoderGroup>(0x20));
  }

  void addDevice(std::shared_ptr<PCF8574> device) {
    inputDevices.push_back(device);
  }

  void read() override {
    for (auto dev : inputDevices) {
      dev->read();
    }
  }
};