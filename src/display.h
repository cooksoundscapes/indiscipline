#include "lua-runner-base.h"
#include "hardware/ssd1306-base.h"
#include <memory>
#include <vector>
#include <iostream>

class Display {
  int width, height;
  bool shouldQuit = false;
  std::vector<uint8_t> pixel_data;

  std::shared_ptr<LuaRunnerBase> luaInterpreter;
  std::shared_ptr<SSD1306Base> device;

  void draw(int stride);
  void measureFps();
  
public:
  Display(int w, int h) : width(w), height(h) {}

  void setSize(int w, int h) {
    this->width = w;
    this->height = h;
  }
  
  void setDevice(std::shared_ptr<SSD1306Base> device) {this->device = device;}
  
  void setLuaInterpreter(std::shared_ptr<LuaRunnerBase> LIntr) {
    this->luaInterpreter = LIntr;
  }

  void stop() {
    std::cout << "bOOOOHBYE\n";
    shouldQuit = true;
  }

  void loop();
  void loadLuaScript(std::string file);
};
