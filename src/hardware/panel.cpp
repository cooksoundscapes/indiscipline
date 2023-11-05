#include "panel.h"

Panel::Panel() {
	inputDevices.insert({"encoders", std::make_shared<EncoderGroup>(0x20)});
	
	// define callback lambdas
	GPIO::InputCallback buttonAction = [this](std::string name, int level){
		oscSend(name, level);
	};

	GPIO::InputCallback readI2CDevice = [this](std::string device, int level){
		if (level == 0) {
			auto levels = read(device);
			size_t i{0};
			for (auto l : levels) {
				i++;
				if (l != 0)
					oscSend(device + "/" + std::to_string(i), l);
			}
		}
	};
			
  this->gpio = std::make_shared<GPIO>();
  gpio->addController(23, {"system", buttonAction});
  gpio->addController(24, {"left", buttonAction});
  gpio->addController(25, {"right", buttonAction});
  gpio->addController(18, {"encoders", readI2CDevice});
}

void Panel::oscSend(std::string path, int value) {
	lo_send(osc_addr, path.c_str(), "f", (float)value);
}

