#include "screen-base.h"
#include "hardware/ssd1306-base.h"
#include <memory>
#include <vector>
#include <iostream>
#include <chrono>
#include "main.h"

using namespace std::chrono;

class SPIDisplay : public ScreenBase {
  std::vector<uint8_t> pixel_data;

  std::shared_ptr<SSD1306Base> device;

  void draw(int stride);

  milliseconds frameDuration{1000 / TARGET_FPS};
  
public:
  SPIDisplay(int w, int h) : ScreenBase(w, h) {}
  
  void setDevice(std::shared_ptr<SSD1306Base> device) {this->device = device;}

  void setFps(int fps) override {
    frameDuration = milliseconds(1000 / fps);
  }

  void loop() override;
};
