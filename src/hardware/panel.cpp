#include "panel.h"

Panel::Panel() {
	inputDevices.insert({"encoders", std::make_shared<EncoderGroup>(0x20)});
	
	// define callback lambdas
	GPIO::InputCallback buttonAction = [this](std::string name, int level){
		std::cout << "Button " << name << " is at " << level << '\n';
	};
	GPIO::InputCallback readDevice = [this](std::string device, int level){
		if (level == 0) {
			auto levels = read(device);
		}
	};
			
  this->gpio = std::make_shared<GPIO>();
  gpio->addController(23, {"system", buttonAction});
  gpio->addController(24, {"left", buttonAction});
  gpio->addController(25, {"right", buttonAction});
  gpio->addController(18, {"encoders", readDevice});
}

