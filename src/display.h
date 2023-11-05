#include "lua-runner-base.h"
#include <memory>
#include <vector>

class Display {
  std::shared_ptr<LuaRunnerBase> luaInterpreter;

  int width, height;
  bool shouldQuit = false;
  std::vector<unsigned char> pixel_data;

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