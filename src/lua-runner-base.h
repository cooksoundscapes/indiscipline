#pragma once
#include <string>
#include <lua.h>
#include <vector>
#include <mutex>

class LuaRunnerBase
{
protected:
  std::string ipTarget;

  struct MouseData {
    int x, y, button;
  };
  static MouseData mouse;
  static std::mutex mouseMux;

public:
  virtual void loadFile(std::string file) = 0;
  virtual void setGlobal(std::string, double) = 0;
  virtual void draw() = 0;

  void setIPTarget(std::string ip) {ipTarget = ip;}

  struct Param {
    char type;
    float f_value;
    const char* s_value;
  };
  virtual void callFunction(std::string, std::vector<Param>&) = 0;
  virtual void setTable(std::string, std::vector<float>&) = 0;
  virtual void triggerPanelCallback(std::string device, int pin, int value) = 0;

  virtual void resetLuaState() = 0;
  virtual void schedulePrint() = 0;

  static void setMousePosition(int x, int y) {
    std::lock_guard<std::mutex> guard(mouseMux);
    mouse.x = x;
    mouse.y = y;
  }
  static void setMouseButton(int b) {
    std::lock_guard<std::mutex> guard(mouseMux);
    mouse.button = b;
  }
  static MouseData getMouse() {
    std::lock_guard<std::mutex> guard(mouseMux);
    return mouse;
  }
};