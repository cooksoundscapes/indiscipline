#pragma once
#include "main.h"
#include <lua.hpp>
#include <string>
#include <memory>
#include <mutex>
#include <lo/lo.h>
#include <atomic>
#include "lua-runner-base.h"
#include "audio-sink-base.h"
#include "hardware/panel-base.h"

//----functions to be registered in lua state
extern int _set_source_rgb(lua_State* l); 
extern int _set_source_rgba(lua_State* l); 
extern int _new_path(lua_State* l);
extern int _close_path(lua_State* l);
extern int _rectangle(lua_State* l);
extern int _arc(lua_State* l);
extern int _move_to(lua_State* l);
extern int _line_to(lua_State* l);
extern int _rel_move_to(lua_State* l);
extern int _rel_line_to(lua_State* l);
extern int _set_line_width(lua_State* l);
extern int _paint(lua_State* l);
extern int _fill(lua_State* l);
extern int _fill_preserve(lua_State* l);
extern int _stroke(lua_State* l);
extern int _set_operator(lua_State* l);
extern int _text(lua_State* l);
extern int _create_surface(lua_State* l);
extern int _draw_surface(lua_State* l);
extern int _destroy_surface(lua_State* l);
extern int _set_line_cap(lua_State* l);
extern int _hex_to_rgb(lua_State* l);

class LuaRunner : public LuaRunnerBase {
  lua_State* state;
  std::shared_ptr<AudioSinkBase> audioSink;
  std::shared_ptr<PanelBase> panel;

  std::string projectPath, defaultPage, currentPage;
 
  void setCurrentPage(std::string p);

  std::recursive_mutex mutex;
  int mouseX{0}, mouseY{0}, mouseButton{0};

  lo_address client_osc_addr;

  void defineCallbacks();

  int screen_w, screen_h;
  bool shouldPrint = false;

public:
  LuaRunner(int w, int h, std::string path, std::string ipTarget, std::string defaultPage);
  ~LuaRunner();

  void init();

  void setAudioSink(std::shared_ptr<AudioSinkBase> audsnk) { this->audioSink = audsnk; }
  void setPanel(std::shared_ptr<PanelBase> panel) { 
    this->panel = panel;
    panel->registerCallback(SEND_OSC, sendOsc);
    panel->registerCallback(DIRECT_CONTROL, directControl);
  }

  void loadFile(std::string file) override;
  void setGlobal(std::string name, double value) override;
  void setGlobal(std::string name, std::string value);

  void draw() override;
  void loadFunction(std::string name, lua_CFunction fn);
  void callFunction(std::string, std::vector<Param>&) override;
  void callFunction(std::string);
  void setTable(std::string, std::vector<float>&) override;
  
  void schedulePrint() override {
    shouldPrint = true;
  }

  void resetLuaState() override;

  void triggerPanelCallback(std::string device, int pin, int value) override;

  void setIPTarget(std::string ip) {
    LuaRunnerBase::setIPTarget(ip);
    defineCallbacks();
  }
  
  void setProjectPath(std::string path) {
	  projectPath = path;
  }

  void setScreenSize(int w, int h) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    screen_w = w;
    screen_h = h;
    setGlobal(SCREEN_W, w);
    setGlobal(SCREEN_H, h);
  }
  
  std::string getPath() {return projectPath;}

  void setMousePos(int x, int y) override {
    mouseX = x;
    mouseY = y;
  }

  void setMouseButton(int s) override {
    mouseButton = s;
  }

  // callback control functions
  std::function<void(std::string, int, int)> sendOsc;
  std::function<void(std::string, int, int)> directControl;

  // registerable lua function with private access
  static int getAudioBuffer(lua_State*);
  static int loadModule(lua_State*);
  static int getBufferSize(lua_State*);
  static int startJack(lua_State*);
  static int stopJack(lua_State*);
  static int setPanelLights(lua_State*);
  static int setOSCTarget(lua_State*);
  static int getMouseData(lua_State*);
};
