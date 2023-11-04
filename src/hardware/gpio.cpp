#include "gpio.h"
#include <iostream>

GPIO::GPIO() {
  if (gpioInitialise() < 0) {
    std::cerr << "Failed to initialize GPIO library;\n";
    exit(1);
  }
}

GPIO::~GPIO() {
  for (auto pin : pins) {
    gpioSetAlertFuncEx(pin.first, 0, 0);
  }
  gpioTerminate();
}

void GPIO::addController(int pin, controller ctrl, unsigned mode) {
  gpioSetMode(pin, mode);
  if (mode == PI_INPUT) {
    gpioSetPullUpDown(pin, PI_PUD_UP);
  }
  gpioSetAlertFuncEx(pin, default_callback, this);
  pins.insert({pin, ctrl});
}

void GPIO::default_callback(int pin, int level, uint32_t tick, void* userData) {
  auto gpio = (GPIO*)userData;
  auto& ctrl = gpio->pins[pin];
  ctrl.action(ctrl.name, level);
}
