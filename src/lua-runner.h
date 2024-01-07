#pragma once
#include "main.h"
#include <lua.hpp>
#include <string>
#include <memory>
#include <mutex>
#include "lua-runner-base.h"
#include "audio-sink-base.h"
#include "hardware/panel-base.h"

//----functions to be registered in lua state
extern int _set_source_rgb(lua_State* l); 
extern int _set_source_rgba(lua_State* l); 
extern int _rectangle(lua_State* l);
extern int _arc(lua_State* l);
extern int _move_to(lua_State* l);
extern int _line_to(lua_State* l);
extern int _rel_move_to(lua_State* l);
extern int _rel_line_to(lua_State* l);
extern int _set_line_width(lua_State* l);
extern int _paint(lua_State* l);
extern int _fill(lua_State* l);
extern int _stroke(lua_State* l);
extern int _set_operator(lua_State* l);
extern int _text(lua_State* l);
extern int _create_surface(lua_State* l);
extern int _draw_surface(lua_State* l);
extern int _destroy_surface(lua_State* l);

extern int hexToRGB(lua_State* l);

class LuaRunner : public LuaRunnerBase {
  lua_State* state;
  std::shared_ptr<AudioSinkBase> audioSink;
  std::shared_ptr<PanelBase> panel;

  std::string currentPage = HOME_PAGE;
  void setCurrentPage(std::string p);
  void setDirectPanelControl();

  std::recursive_mutex mutex;

  bool firstLoaded = false;

public:
  LuaRunner();
  ~LuaRunner();

  void setAudioSink(std::shared_ptr<AudioSinkBase> audsnk) { this->audioSink = audsnk; }
  void setPanel(std::shared_ptr<PanelBase> panel) { 
    this->panel = panel;
    setDirectPanelControl();
  }

  void loadFile(std::string file);
  void setGlobal(std::string name, double value);

  void draw() override;
  void loadFunction(std::string name, lua_CFunction fn);
  void callFunction(std::string, std::vector<Param>&) override;
  void callFunction(std::string);
  void setTable(std::string, std::vector<float>&) override;

  static std::string getPath();

  // registerable lua function with private access
  static int getAudioBuffer(lua_State*);
  static int loadModule(lua_State*);
  static int getBufferSize(lua_State*);
  static int startJack(lua_State*);
  static int stopJack(lua_State*);
  static int setPanelLights(lua_State*);
};