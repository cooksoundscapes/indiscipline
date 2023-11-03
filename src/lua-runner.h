#pragma once
#include <lua.hpp>
#include <string>
#include <memory>
#include "lua-runner-abstract.h"
#include "audio-sink-abstract.h"

class LuaRunner : public AbstractLuaRunner {
  lua_State* state;
  std::shared_ptr<AbstractAudioSink> audioSink;

public:
  LuaRunner();
  ~LuaRunner();

  void setAudioSink(std::shared_ptr<AbstractAudioSink> audsnk) { this-> audioSink = audsnk; }

  void loadFile(std::string file);

  void setGlobal(std::string name, int value);
  void setGlobal(std::string name, float value);

  void draw() override;
  void loadFunction(std::string name, lua_CFunction fn);
  void callFunction(std::string name, std::vector<Param> params) override;

  // registerable lua function with private access
  static int getAudioBuffer(lua_State*);
};