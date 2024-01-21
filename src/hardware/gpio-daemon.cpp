#include "gpio.h"
#include <pigpiod_if2.h>

GPIO::GPIO() {
	daemon = pigpio_start(0, 0);
	if (daemon < 0) {
		std::cerr << "Failed to initialize GPIO daemon;\n";
    exit(1);
	}
}

GPIO::~GPIO() {
  for (auto cb : callback_ids) {
		callback_cancel(cb);
	}
  pigpio_stop(daemon);
}

void GPIO::setPin(int pin, unsigned level) {
	gpio_write(daemon, pin, level);
}

void GPIO::addInterrupt(int pin, controller ctrl) {
	set_mode(daemon, pin, PI_INPUT);
	set_pull_up_down(daemon, pin, PI_PUD_UP);
	int cb_id = callback_ex(daemon, pin, EITHER_EDGE, default_callback, this);
	callback_ids.push_back(cb_id);
  pins.insert({pin, ctrl});
}

void GPIO::default_callback(int daemon, unsigned int pin, unsigned int level, unsigned int tick, void* userData) {
  auto gpio = (GPIO*)userData;
  auto& ctrl = gpio->pins[pin];
  ctrl.action(ctrl.name, level);
}
