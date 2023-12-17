#include "screen-base.h"
#include "hardware/ssd1306-base.h"
#include <memory>
#include <vector>
#include <iostream>

class SPIDisplay : public ScreenBase {
  std::vector<uint8_t> pixel_data;

  std::shared_ptr<SSD1306Base> device;

  void draw(int stride);
  void measureFps();

  int targetFps = 60;
  
public:
  SPIDisplay(int w, int h) : ScreenBase(w, h) {}  
  
  void setDevice(std::shared_ptr<SSD1306Base> device) {this->device = device;}

  void loop() override;
};
