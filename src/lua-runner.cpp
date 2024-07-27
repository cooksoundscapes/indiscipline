#include "lua-runner.h"
#include <iostream>
#include <vector>
#include <cstdlib>

LuaRunner::LuaRunner(int w, int h, std::string path, std::string ip, std::string page) {
  screen_w = w;
  screen_h = h;
  projectPath = path;
  ipTarget = ip;
  defaultPage = page;
  currentPage = page;

  // define control callbacks
  defineCallbacks();
}

void LuaRunner::init() {
  state = luaL_newstate();
  luaL_openlibs(state);
  std::cout << "Initialized Lua API - version " << lua_version(NULL) << std::endl;

  // expose lua interpreter class to the scripts
  lua_pushlightuserdata(state, this);
  lua_setglobal(state, "LuaInterpreter");
  // register screen size globals for UI reference
  lua_pushnumber(state, screen_w);
  lua_setglobal(state, SCREEN_W);
  lua_pushnumber(state, screen_h);
  lua_setglobal(state, SCREEN_H);

  // cairo drawing functions
  loadFunction("set_source_rgb", &_set_source_rgb);
  loadFunction("set_source_rgba", &_set_source_rgba);
  loadFunction("new_path", &_new_path);
  loadFunction("close_path", &_close_path);
  loadFunction("rectangle", &_rectangle);
  loadFunction("arc", &_arc);
  loadFunction("move_to", &_move_to);
  loadFunction("line_to", &_line_to);
  loadFunction("rel_move_to", &_rel_move_to);
  loadFunction("rel_line_to", &_rel_line_to);
  loadFunction("text", &_text);
  loadFunction("paint", &_paint);
  loadFunction("fill", &_fill);
  loadFunction("fill_preserve", &_fill_preserve);
  loadFunction("stroke", &_stroke);
  loadFunction("set_operator", &_set_operator);
  loadFunction("set_line_width", &_set_line_width);
  loadFunction("create_surface", &_create_surface);
  loadFunction("draw_surface", &_draw_surface);
  loadFunction("destroy_surface", &_destroy_surface);
  loadFunction("set_line_cap", &_set_line_cap);
  loadFunction("hex", &_hex_to_rgb);

  // those depend on a self reference
  loadFunction("get_audio_buffer", &LuaRunner::getAudioBuffer);
  loadFunction("get_buffer_size", &LuaRunner::getBufferSize);
  loadFunction("load_module", &LuaRunner::loadModule);
  loadFunction("jack_start", &LuaRunner::startJack);
  loadFunction("jack_stop", &LuaRunner::stopJack);
  loadFunction("set_lights", &LuaRunner::setPanelLights);
  loadFunction("set_osc_target", &LuaRunner::setOSCTarget);

  // include project path to LUA_PATH
  std::string setPkgCommand = "package.path = \"" + projectPath + "?.lua;" + "\" .. package.path";
  luaL_dostring(state, setPkgCommand.c_str());

  // include project path to LUA_C_PATH
  setPkgCommand = "package.cpath = \"" + projectPath + "?.so;" + "\" .. package.cpath";
  luaL_dostring(state, setPkgCommand.c_str());

  // load $lua_path/setup.lua
  std::string luaSetupPath = projectPath + LUA_SETUP + ".lua";
  if (luaL_dofile(state, luaSetupPath.c_str()) == 0) {
    std::cout << "Successfully loaded " << luaSetupPath << std::endl;
  } else {
    std::cerr << "Failed to load script " << luaSetupPath << ": " << lua_tostring(state, -1) << std::endl;
  }

  loadFile(defaultPage);
}

LuaRunner::~LuaRunner() {
  lua_close(state);
}

void LuaRunner::resetLuaState() {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  if (state == nullptr) return;

  lua_close(state); // close state
  init(); // call the constructor again
}

void LuaRunner::defineCallbacks() {
  // check if target is other than localhost
  if (ipTarget.empty()) {
    client_osc_addr = lo_address_new(NULL, OSC_CLIENT);
  } else {
    std::cout << "OSC messages will be sent to address " << ipTarget << '\n';
    client_osc_addr = lo_address_new(ipTarget.c_str(), OSC_CLIENT);
  }

  // Define callback to be sent at every page (other than home)
  sendOsc = [this](std::string device, int pin, int value)
  {
    // bypass osc if home button is pressed
    if (device == NAV_BUTTONS && pin == HOME_BUTTON && value == PRESS_VALUE) {
      loadFile(HOME_PAGE);
      return;
    }
    auto path = "/" + device + "/" + std::to_string(pin);
    lo_send(client_osc_addr, path.c_str(), "f", (float)value);
  };

  // Define callback to be sent on home page
  directControl = [this](std::string device, int pin, int value)
  {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    lua_getglobal(state, PANEL_INPUT);
    if (lua_isfunction(state, -1)) {
      lua_pushstring(state, device.c_str());
      lua_pushnumber(state, pin);
      lua_pushnumber(state, value);
      if (lua_pcall(state, 3, 0, 0) != 0) {
        std::cerr << "Lua error: " << lua_tostring(state, -1) << std::endl;
      }
    }
  };
}

// unprotected!
void LuaRunner::globalFunction(const char* fn) {
  lua_getglobal(state, fn);
  if (lua_isfunction(state, -1)) {
    if (lua_pcall(state, 0, 0, 0) != 0) {
      std::cerr << "Lua error: " << lua_tostring(state, -1) << std::endl;
    }
  }
}

void LuaRunner::loadFile(std::string file)
{
  std::lock_guard<std::recursive_mutex> lock(mutex);

  globalFunction(CLEANUP);

  auto filepath = projectPath + file + ".lua";

  if (luaL_dofile(state, filepath.c_str()) != 0) {
    std::cerr << "Failed to load script " << filepath << ": " << lua_tostring(state, -1) << std::endl;
  }

  setCurrentPage(file);
}

void LuaRunner::setGlobal(std::string varname, double value) {
  std::lock_guard<std::recursive_mutex> lock(mutex);

  lua_pushnumber(state, value);
  lua_setglobal(state, varname.c_str());
}

void LuaRunner::setGlobal(std::string varname, std::string value) {
  std::lock_guard<std::recursive_mutex> lock(mutex);

  lua_pushstring(state, value.c_str());
  lua_setglobal(state, varname.c_str());
}

void LuaRunner::setGlobal(std::string varname, void* userData) {
  std::lock_guard<std::recursive_mutex> lock(mutex);

  lua_pushlightuserdata(state, userData);
  lua_setglobal(state, varname.c_str());
}

void LuaRunner::draw() {
  std::lock_guard<std::recursive_mutex> lock(mutex);

  #ifndef USE_FB
  #ifndef USE_SSD1306
    lua_pushnumber(state, mouseX);
    lua_setglobal(state, "mouse_x");
    lua_pushnumber(state, mouseY);
    lua_setglobal(state, "mouse_y");
    lua_pushnumber(state, mouseButton);
    lua_setglobal(state, "mouse_button");
  #endif
  #endif

  if (resizing) {
    resizing = false;
    lua_pushnumber(state, screen_w);
    lua_setglobal(state, SCREEN_W);
    lua_pushnumber(state, screen_h);
    lua_setglobal(state, SCREEN_H);
  }

  globalFunction(DRAW);
}

void LuaRunner::loadFunction(std::string name, lua_CFunction fn)
{
  std::lock_guard<std::recursive_mutex> lock(mutex);

  lua_pushcfunction(state, fn);
  lua_setglobal(state, name.c_str());
}

void LuaRunner::callFunction(std::string name, std::vector<Param>& params)
{
  std::lock_guard<std::recursive_mutex> lock(mutex);

  lua_getglobal(state, name.c_str());
  if (lua_isfunction(state, -1)) {
    // push arguments
    for (const auto& p : params) {
      if (p.type == 'f')
        lua_pushnumber(state, p.f_value);
      else if (p.type == 's')
        lua_pushstring(state, p.s_value); 
    }

    int argcount = params.size();
    if (lua_pcall(state, argcount, 0, 0) != 0) {
      std::cerr << "Lua error: " << lua_tostring(state, -1) << std::endl;
    }
  }
}

void LuaRunner::setTable(std::string name, std::vector<float>& buff)
{
  std::lock_guard<std::recursive_mutex> lock(mutex);

  lua_getglobal(state, SET_TABLE);
  if (!lua_isfunction(state, -1)) {
    lua_pop(state, 1);
    return;
  }
  lua_pushstring(state, name.c_str());

  lua_newtable(state);
  size_t i{0};
  for (auto f : buff) {
    lua_pushinteger(state, i + 1);
    lua_pushnumber(state, f);
    lua_settable(state, -3);
    i++;
  }

  if (lua_pcall(state, 2, 0, 0) != 0) {
    const char* errorMsg = lua_tostring(state, -1);
    std::cerr << "Error calling Lua function: " << errorMsg << '\n';
    lua_pop(state, 1); // Pop the error message from the stack
  }
}

void LuaRunner::setCurrentPage(std::string page) {
  currentPage = page;
  if (!panel) return;

  if (page == HOME_PAGE && panel) {
    panel->setCurrentCallback(DIRECT_CONTROL);
  } else if (panel) {
    panel->setCurrentCallback(SEND_OSC);
  }
}

// this is used mostly for debugging purposes
void LuaRunner::triggerPanelCallback(std::string device, int pin, int value)
{
  if (currentPage == HOME_PAGE) {
    directControl(device, pin, value);
  } else {
    sendOsc(device, pin, value);
  }
}

/**
 * Static Calls to be registered at State
 */

// retrieve audio buffer function from sink
int LuaRunner::getAudioBuffer(lua_State* l) {
  lua_getglobal(l, "LuaInterpreter");
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  lua_check_num_args(l, 1);
  double channel = luaL_checknumber(l, 1);

  auto buffer = luaRunner->audioSink->getBuffer(channel);
  lua_newtable(l);

  if (buffer.size() > 0) {
    for (int i{0}; i < buffer.size(); i++) {
      lua_pushnumber(l, buffer[i]);
      lua_rawseti(l, -2, i + 1);
    }
  }
  return 1;
} 

int LuaRunner::getBufferSize(lua_State* l) {
  lua_getglobal(l, "LuaInterpreter");
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  int b_size = luaRunner->audioSink->getBufferSize();
  lua_pushnumber(l, b_size);
  return 1;
}

int LuaRunner::startJack(lua_State* l) {
  lua_getglobal(l, "LuaInterpreter");
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, -1));
  lua_pop(l, 1);
  luaRunner->audioSink->start();
  return 0;
}

int LuaRunner::stopJack(lua_State* l) {
  lua_getglobal(l, "LuaInterpreter");
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, -1));
  lua_pop(l, 1);
  luaRunner->audioSink->stop();
  return 0;
}

int LuaRunner::setPanelLights(lua_State* l) {
  lua_getglobal(l, "LuaInterpreter");
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  lua_check_num_args(l, 1);
  int state = luaL_checknumber(l, 1);
  std::bitset<8> bitState(state);
  luaRunner->panel->setDeviceState(LED_ARRAY, bitState);
  return 0;

}

int LuaRunner::loadModule(lua_State* l) {
  lua_getglobal(l, "LuaInterpreter");
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  lua_check_num_args(l, 1);
  const char* file = luaL_checkstring(l, 1);

  luaRunner->setCurrentPage(file);
  auto path = luaRunner->getPath() + file + ".lua";

  lua_getglobal(l, CLEANUP);
  if (lua_isfunction(l, -1)) {
    if (lua_pcall(l, 0, 0, 0) != 0) {
      std::cerr << "Lua error: " << lua_tostring(l, -1) << std::endl;
    }
  }

  if (luaL_dofile(l, path.c_str()) != 0) {
    std::cerr << "Failed to load script " << file << ".lua: " << lua_tostring(l, -1) << std::endl;
  }

  return 0;
}

int LuaRunner::setOSCTarget(lua_State* l) {
  lua_getglobal(l, "LuaInterpreter");
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  lua_check_num_args(l, 1);
  const char* ip_addr = luaL_checkstring(l, 1);

  luaRunner->setIPTarget(ip_addr);
  return 0;
}
