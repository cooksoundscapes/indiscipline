#pragma once
#include <vector>

class SSD1306Base {
public:
  virtual void update(std::vector<unsigned char>) = 0;
};