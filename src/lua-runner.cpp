#include "lua-runner.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include "cairo-wrapper.h"

LuaRunner::LuaRunner(int w, int h, std::string path, std::string ip) {
  projectPath = path;
  screen_w = w;
  screen_h = h;
  ipTarget = ip;
  // initialize lua state and register all globals and functions
  init();

  // define control callbacks
  defineCallbacks();
}

void LuaRunner::init() {
  state = luaL_newstate();
  luaL_openlibs(state);
  std::cout << "Initialized Lua API - version " << lua_version(NULL) << std::endl;

  // expose lua interpreter class to the scripts
  lua_pushlightuserdata(state, this);
  lua_setglobal(state, "app");
  // register screen size globals for UI reference
  setGlobal(SCREEN_W, screen_w);
  setGlobal(SCREEN_H, screen_h);

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
  std::string setPkgCommand = "package.path = \"" + projectPath + "?.lua;\" .. package.path";
  luaL_dostring(state, setPkgCommand.c_str());

  // load $lua_path/setup.lua
  std::string luaSetupPath = projectPath + LUA_SETUP + ".lua";
  if (luaL_dofile(state, luaSetupPath.c_str()) == 0) {
    std::cout << "Successfully loaded " << luaSetupPath << std::endl;
  } else {
    std::cerr << "Failed to load script " << luaSetupPath << ": " << lua_tostring(state, -1) << std::endl;
  }
}

LuaRunner::~LuaRunner() {
  lua_close(state);
}

void LuaRunner::resetLuaState() {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  if (state == nullptr) return;

  lua_close(state); // close state
  init(); // call the constructor again
  loadFile(HOME_PAGE); // load home
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
    if (device == NAV_BUTTONS && pin == HOME_BUTTON && value == 1) {
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

void LuaRunner::loadFile(std::string file)
{
  std::lock_guard<std::recursive_mutex> lock(mutex);

  // call "Cleanup" function declared at lua scripts
  lua_getglobal(state, CLEANUP);
  if (lua_isfunction(state, -1)) {
    if (lua_pcall(state, 0, 0, 0) != 0) {
      std::cerr << "Lua error: " << lua_tostring(state, -1) << std::endl;
    }
  }

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

void LuaRunner::draw() {
  std::lock_guard<std::recursive_mutex> lock(mutex);

  lua_getglobal(state, DRAW);

  if (lua_pcall(state, 0, 0, 0) != 0) {
    std::cerr << "Lua error: " << lua_tostring(state, -1) << std::endl;
  }
  if (shouldPrint) {
    Cairo::print();
    shouldPrint = false;
  }
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

void LuaRunner::callFunction(std::string name)
{
  std::lock_guard<std::recursive_mutex> lock(mutex);

  lua_getglobal(state, name.c_str());
  if (lua_isfunction(state, -1)) {
    if (lua_pcall(state, 0, 0, 0) != 0) {
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
  lua_newtable(state);
  size_t i{0};
  for (auto f : buff) {
    lua_pushinteger(state, i + 1);
    lua_pushnumber(state, f);
    lua_settable(state, -3);
    i++;
  }
  lua_pushstring(state, name.c_str());

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

void LuaRunner::triggerPanelCallback(std::string device, int pin, int value)
{
  if (currentPage == HOME_PAGE) {
    directControl(device, pin, value);
  } else {
    sendOsc(device, pin, value);
  }
}
