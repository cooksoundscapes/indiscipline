#pragma once
#include <functional>
#include <string>

class GPIOBase {
public:
  virtual void setPin(int, unsigned) = 0;

  using InputCallback = std::function<void(std::string name, int level)>;
  struct controller {
		std::string name;
		InputCallback action;
	};
  virtual void addController(int, controller) = 0;
};
