#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include <pigpio.h>

class GPIO{
public:
  GPIO();
  ~GPIO();
  
	using InputCallback = std::function<void(std::string name, int level)>;
	
  struct controller {
		std::string name;
		InputCallback action;
	};
  void addController(int, controller);

private:
  std::unordered_map<int, controller> pins;
  static void default_callback(int pin, int level, uint32_t tick, void* userData);
};
