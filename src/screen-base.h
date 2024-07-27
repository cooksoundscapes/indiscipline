#pragma once
#include <memory>
#include <iostream>
#include "lua-runner-base.h"

class ScreenBase {
protected:
  std::shared_ptr<LuaRunnerBase> luaInterpreter;

  bool shouldQuit = false;
  int width, height;

public:
  ScreenBase(int w, int h) {
    this->width = w;
    this->height = h;
  }

  void setSize(int w, int h) {
    this->width = w;
    this->height = h;
  }

  virtual ~ScreenBase() {}

  virtual void setLuaInterpreter(std::shared_ptr<LuaRunnerBase> LIntr) {
    this->luaInterpreter = LIntr;
  }

  virtual void loop() = 0;
  virtual void setFps(int) = 0;

  void stop() {
    std::cout << "bOOOOHBYE\n";
    shouldQuit = true;
  }

  std::shared_ptr<LuaRunnerBase> getLuaInterpreter() {
    return luaInterpreter;
  }

  virtual void loadFile(const char*) = 0;
  virtual void schedulePrint() = 0;
};