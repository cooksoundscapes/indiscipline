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
  virtual void setTable(std::string, std::vector<float>&) = 0;
  virtual void triggerPanelCallback(std::string device, int pin, int value) = 0;

  virtual void resetLuaState() = 0;
  virtual void setScreenSize(int, int) = 0;

  virtual void setMousePos(float x, float y) = 0;
  virtual void setMouseButton(int s) = 0;


  // luaState agnostic static calls
  static int getTableIntValue(lua_State* l, int index, const char* k) {
    lua_pushstring(l, k);
    lua_gettable(l, index);
    int v = luaL_checknumber(l, -1);
    lua_pop(l, 1);
    return v;
  }

  static void callFunctionOnStack(lua_State* l, int index, int argc, int n_result) {
    if (!lua_isfunction(l, index)) {
      lua_pushstring(l, "Expected a function");
      lua_error(l);
    }
    // duplicate function and place the clone at the stack's top
    lua_pushvalue(l, index);
    if (lua_pcall(l, argc, n_result, 0) != LUA_OK) {
      const char *error_message = lua_tostring(l, -1);
      lua_pushstring(l, error_message);
      lua_error(l);
    }
  }

  static void callTableFunction(
    lua_State* l,
    int table_index,
    const char* fn_name,
    int argc,
    int n_result
  ) {
    // Preserve the current stack top
    int stack_top = lua_gettop(l);

    lua_pushstring(l, fn_name);
    lua_gettable(l, table_index);

    // Push the table itself as the first argument (self)
    lua_pushvalue(l, table_index);
    argc++;

    callFunctionOnStack(l, -(argc), argc, n_result);

    lua_settop(l, stack_top + n_result);
  }
};