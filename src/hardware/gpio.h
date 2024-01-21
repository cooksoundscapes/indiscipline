#pragma once
#include "gpio-base.h"
#include <unordered_map>
#include <iostream>

class GPIO : public GPIOBase {
public:
  GPIO();
  ~GPIO();
  
  void addInterrupt(int, controller) override;
  void setPin(int pin, unsigned level) override;

private:
	int daemon;
	std::vector<int> callback_ids;

  std::unordered_map<int, controller> pins;
 
  //static void default_callback(int pin, int level, uint32_t tick, void* userData);
  static void default_callback(int daemon, unsigned int pin, unsigned int level, unsigned int tick, void* userData);
};
