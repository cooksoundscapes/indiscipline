#include "lua-runner-base.h"
#include "hardware/ssd1306-base.h"
#include <memory>
#include <vector>

class Display {
  int width, height;
  bool shouldQuit = false;
  std::vector<unsigned char> pixel_data;


  std::shared_ptr<LuaRunnerBase> luaInterpreter;
  std::shared_ptr<SSD1306Base> display;

  void draw(int stride);
  void measureFps();
  
public:
  Display(int w, int h) : width(w), height(h) {}
  
  void setLuaInterpreter(std::shared_ptr<LuaRunnerBase> LIntr) {
    this->luaInterpreter = LIntr;
  }

  void loop();
  void loadLuaScript(std::string file);
};