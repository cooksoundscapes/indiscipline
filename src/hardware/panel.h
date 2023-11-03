#pragma once
#include "pcf-8574.h"
#include "panel-abstract.h"
#include <vector>
#include <memory>

class Panel : public AbstractPanel {
  std::vector<std::shared_ptr<PCF8574>> inputDevices;

public:
  Panel() {
    auto encoders = std::make_shared<EncoderGroup>(0x20);
    addDevice(encoders);
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