#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include <pigpio.h>

class GPIO {
	struct controller {
		std::string name;
		std::function<void(std::string name, int level)> action;
	};
  std::unordered_map<int, controller> pins;

  static void default_callback(int pin, int level, uint32_t tick, void* userData);

public:
  GPIO();
  ~GPIO();

  void addController(int, controller);
};
