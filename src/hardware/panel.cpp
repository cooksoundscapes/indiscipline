#include "panel.h"

Panel::Panel(std::shared_ptr<GPIOBase> gpio)
: gpio{gpio}
{
	inputDevices.insert({ENCODERS, std::make_shared<EncoderGroup>(0x20)});
	inputDevices.insert({BUTTONS, std::make_shared<ButtonArray>(0x21)});
	
	// define callback lambdas
	GPIOBase::InputCallback readI2CDevice = [this](std::string device, int level){
		if (level == 0) {
			auto levels = inputDevices[device]->read();
			size_t i{0};
			for (auto l : levels) {
				i++;
				if (l != 0 && currentCallback)
					(*currentCallback)(device , i, l);
			}
		}
	};
	// setup I2C devices to their INT pins
  gpio->addController(ENCODER_INT, {ENCODERS, readI2CDevice});
	gpio->addController(BUTTON_INT, {BUTTONS, readI2CDevice});
}

void Panel::setCurrentCallback(std::string cb) {
	auto it = callbacks.find(cb);
	if (it != callbacks.end()) {
		currentCallback = std::make_unique<ReadCB>(callbacks[cb]);
	}
}