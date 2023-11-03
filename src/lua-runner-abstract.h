#pragma once
#include <string>
#include <lua.h>
#include <vector>

class AbstractLuaRunner
{
public:
  virtual void loadFile(std::string file) = 0;
  virtual void setGlobal(std::string, int) = 0;
  virtual void setGlobal(std::string, float) = 0;
  virtual void draw() = 0;

  struct Param {
    char type;
    float f_value;
    const char* s_value;
  };
  virtual void callFunction(std::string name, std::vector<Param> params) = 0;
};