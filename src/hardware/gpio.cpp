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

void GPIO::registerPin(std::string name, int pin, unsigned mode) {
  gpioSetMode(pin, mode);
  if (mode == PI_INPUT) {
    gpioSetPullUpDown(pin, PI_PUD_UP);
  }
  gpioSetAlertFuncEx(pin, default_callback, this);
  pins.insert({pin, name});
}

void GPIO::default_callback(int pin, int level, uint32_t tick, void* userData) {
  auto gpio = (GPIO*)userData;
  auto& name = gpio->pins[pin];
  std::cout << "Pin " << name << " is at level " << level << "; ticks(?) value is " << tick << '\n';
}