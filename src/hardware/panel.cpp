#include "panel.h"

Panel::Panel(std::shared_ptr<GPIOBase> gpio)
: gpio{gpio}
{
	// mini mim
	//inputDevices.insert({ENCODERS, std::make_shared<EncoderGroup>(0x20)});
	//inputDevices.insert({NAV_BUTTONS, std::make_shared<ButtonArray>(0x21)});

	// megazord
	inputDevices.insert({NAV_BUTTONS, std::make_shared<ButtonArray>(0x20)}); // GPIO 17
	inputDevices.insert({SEQ_BUTTONS, std::make_shared<ButtonArray>(0x23)}); // GPIO 27
	inputDevices.insert({ENCODERS, std::make_shared<EncoderGroup>(0x27)}); // GPIO 22
	outputDevices.insert({LED_ARRAY, std::make_shared<LightArray>(0x21)}); // GPIO 4

	inputDevices[NAV_BUTTONS]->read();
	inputDevices[SEQ_BUTTONS]->read();
	inputDevices[ENCODERS]->read();
	
	// define callback lambdas
	GPIOBase::InputCallback readI2CDevice = [this](std::string device, int level) {
		if (level == 0) {
			auto levels = inputDevices[device]->read();
			size_t i{0};
			for (auto l : levels) {
				i++;
				if (l != 0 && currentCallback) {
					(*currentCallback)(device , i, l);
				}
			}
		}
	};
	// mini mim
	//gpio->addInterrupt(ENCODER_INT, {ENCODERS, readI2CDevice});
	//gpio->addInterrupt(BUTTON_INT, {BUTTONS, readI2CDevice});
	gpio->addInterrupt(17, {NAV_BUTTONS, readI2CDevice});
	gpio->addInterrupt(27, {SEQ_BUTTONS, readI2CDevice});
	gpio->addInterrupt(22, {ENCODERS, readI2CDevice});
}

void Panel::setCurrentCallback(std::string cb) {
	auto it = callbacks.find(cb);
	if (it != callbacks.end()) {
		currentCallback = std::make_unique<ReadCB>(callbacks[cb]);
	}
}