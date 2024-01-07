#pragma once
#include <iostream>
#include "../main.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <array>

extern "C" {
	#include <linux/i2c-dev.h>
	#include <i2c/smbus.h>	
}

class DeviceI2C {
protected:
	int file;

public:
	DeviceI2C(int address) {
		std::string filename{I2C_DEVICE};
		file = open(filename.c_str(), O_RDWR);
		if (file < 0) {
			std::cout << "Error opening i2c device\n";
			return;
		}
		if (ioctl(file, I2C_SLAVE, address) < 0) {
			std::cout << "Error opening device" << address << '\n';
			return;
		}
	}

	virtual ~DeviceI2C() {
		close(file);
	}	
};
