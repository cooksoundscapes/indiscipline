#pragma once
#include <unordered_map>
#include <string>
#include <pigpio.h>

class GPIO {
  std::unordered_map<int, std::string> pins;

  static void default_callback(int pin, int level, uint32_t tick, void* userData);

public:
  GPIO();
  ~GPIO();

  void registerPin(std::string, int, unsigned);
};