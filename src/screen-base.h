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

  void setLuaInterpreter(std::shared_ptr<LuaRunnerBase> LIntr) {
    this->luaInterpreter = LIntr;
  }

  void loadLuaScript(std::string file) {
    luaInterpreter->loadFile(file);
  }

  virtual void loop() = 0;

  void stop() {
    std::cout << "bOOOOHBYE\n";
    shouldQuit = true;
  }

};