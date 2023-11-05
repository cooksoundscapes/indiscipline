#pragma once
#include <lua.hpp>
#include <string>
#include <memory>
#include "lua-runner-base.h"
#include "audio-sink-base.h"

class LuaRunner : public LuaRunnerBase {
  lua_State* state;
  std::shared_ptr<AudioSinkBase> audioSink;

public:
  LuaRunner();
  ~LuaRunner();

  void setAudioSink(std::shared_ptr<AudioSinkBase> audsnk) { this-> audioSink = audsnk; }

  void loadFile(std::string file);

  void setGlobal(std::string name, double value);

  void draw() override;
  void loadFunction(std::string name, lua_CFunction fn);
  void callFunction(std::string name, std::vector<Param> params) override;

  // registerable lua function with private access
  static int getAudioBuffer(lua_State*);
};