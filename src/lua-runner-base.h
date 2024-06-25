#pragma once
#include <string>
#include <lua.h>
#include <vector>
#include <mutex>

class LuaRunnerBase
{
protected:
  std::string ipTarget;

public:
  virtual void loadFile(std::string file) = 0;
  virtual void setGlobal(std::string, double) = 0;
  virtual void setGlobal(std::string, std::string) = 0;
  virtual void draw() = 0;

  void setIPTarget(std::string ip) {ipTarget = ip;}

  struct Param {
    char type;
    float f_value;
    const char* s_value;
  };
  using ParamList = std::vector<Param>;

  virtual void callFunction(std::string, std::vector<Param>&) = 0;
  virtual void setTable(std::string, std::vector<float>&) = 0;
  virtual void triggerPanelCallback(std::string device, int pin, int value) = 0;

  virtual void resetLuaState() = 0;
  virtual void schedulePrint() = 0;
  virtual void setScreenSize(int, int) = 0;

  virtual void setMousePos(float x, float y) = 0;
  virtual void setMouseButton(int s) = 0;
};