#include "gpio.h"
#include <iostream>
#include <pigpio.h>

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

void GPIO::addController(int pin, controller ctrl) {
  gpioSetMode(pin, PI_INPUT);
  gpioSetPullUpDown(pin, PI_PUD_UP);
  gpioSetAlertFuncEx(pin, default_callback, this);
  pins.insert({pin, ctrl});
}

void GPIO::default_callback(int pin, unsigned level, uint32_t tick, void* userData) {
  auto gpio = (GPIO*)userData;
  auto& ctrl = gpio->pins[pin];
  ctrl.action(ctrl.name, level);
}

void GPIO::setPin(int pin, unsigned level) {
	gpioWrite(pin, level);
}

void GPIO::initOutputPin(int pin) {
	gpioSetMode(pin, PI_OUTPUT);
}
