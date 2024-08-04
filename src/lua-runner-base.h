#pragma once
#include <string>
#include <lua.hpp>
#include <vector>
#include <mutex>
#include <iostream>

class LuaRunnerBase
{
protected:
  std::string ipTarget;

public:
  virtual void loadFile(std::string file) = 0;
  virtual void setGlobal(std::string, double) = 0;
  virtual void setGlobal(std::string, std::string) = 0;
  virtual void setGlobal(std::string name, void* userData) = 0;
  virtual void loadFunction(std::string name, lua_CFunction fn) = 0;
  virtual void updateGlobalVars() = 0;

  void setIPTarget(std::string ip) {ipTarget = ip;}

  struct Param {
    char type;
    float f_value;
    const char* s_value;
  };
  using ParamList = std::vector<Param>;

  virtual void callFunction(std::string, std::vector<Param>&) = 0;
  virtual void callFunction(std::string) = 0;

  virtual void setTable(std::string, std::vector<float>&) = 0;
  virtual void triggerPanelCallback(std::string device, int pin, int value) = 0;
  virtual void callTableRefFunction(int tableRef, const char* fn_name) = 0;

  virtual void resetLuaState() = 0;
  virtual void setScreenSize(int, int) = 0;

  virtual void setMousePos(float x, float y) = 0;
  virtual void setMouseButton(int s) = 0;

  // luaState agnostic static calls
  static int getTableIntValue(lua_State* l, int index, const char* k);
  static bool getTableBoolValue(lua_State* l, int index, const char* k);
  static void callFunctionOnStack(lua_State* l, int index, int argc, int n_result);
  static void callTableFunction(
    lua_State* l,
    int table_index,
    const char* fn_name,
    int argc,
    int n_result
  );
  static void printTypeAtIndex(lua_State* l, int);
  static void printStackSize(lua_State*);
};