#include "lua-runner.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>

LuaRunner::LuaRunner() {
  state = luaL_newstate();
  luaL_openlibs(state);
  std::cout << "Initialized Lua API - version " << lua_version(NULL) << std::endl;

  // expose lua interpreter class to the scripts
  lua_pushlightuserdata(state, this);
  lua_setglobal(state, "app");

  loadFunction("set_source_rgb", &_set_source_rgb);
  loadFunction("set_source_rgba", &_set_source_rgba);
  loadFunction("rectangle", &_rectangle);
  loadFunction("arc", &_arc);
  loadFunction("move_to", &_move_to);
  loadFunction("line_to", &_line_to);
  loadFunction("rel_move_to", &_rel_move_to);
  loadFunction("rel_line_to", &_rel_line_to);
  loadFunction("text", &_text);
  loadFunction("paint", &_paint);
  loadFunction("fill", &_fill);
  loadFunction("stroke", &_stroke);
  loadFunction("set_line_width", &_set_line_width);
  loadFunction("create_surface", &_create_surface);
  loadFunction("draw_surface", &_draw_surface);
  loadFunction("destroy_surface", &_destroy_surface);
  loadFunction("hex", &hexToRGB);
  loadFunction("get_audio_buffer", &LuaRunner::getAudioBuffer);
  loadFunction("load_module", &LuaRunner::loadModule);
}

LuaRunner::~LuaRunner() {
  lua_close(state);
}

std::string LuaRunner::getPath() {
  std::filesystem::path currentPath = std::filesystem::current_path();
  std::string scriptsDir = currentPath.parent_path().string() + "/scripts/";
  return scriptsDir;
}

void LuaRunner::loadFile(std::string file)
{
  std::lock_guard<std::mutex> lock(mutex);

  auto scriptsDir = getPath();
  auto filepath = scriptsDir + file + ".lua";
  std::string utils = scriptsDir + LUA_UTILS + ".lua";

  //load global utils
  if (!firstLoaded) {
    if (luaL_dofile(state, utils.c_str()) == 0) {
      std::cout << "Successfully loaded " << utils << std::endl;
    } else {
      std::cerr << "Failed to load script " << utils << ": " << lua_tostring(state, -1) << std::endl;
    }
    firstLoaded = true;
  }

  lua_getglobal(state, CLEANUP);
  if (lua_isfunction(state, -1)) {
    if (lua_pcall(state, 0, 0, 0) != 0) {
      std::cerr << "Lua error: " << lua_tostring(state, -1) << std::endl;
    }
  }

  if (luaL_dofile(state, filepath.c_str()) != 0) {
    std::cerr << "Failed to load script " << filepath << ": " << lua_tostring(state, -1) << std::endl;
  }

  setCurrentPage(file);
}

void LuaRunner::setGlobal(std::string varname, double value) {
  std::lock_guard<std::mutex> lock(mutex);

  lua_pushnumber(state, value);
  lua_setglobal(state, varname.c_str());
}

void LuaRunner::draw() {
  std::lock_guard<std::mutex> lock(mutex);

  lua_getglobal(state, DRAW);

  if (lua_pcall(state, 0, 0, 0) != 0) {
    std::cerr << "Lua error: " << lua_tostring(state, -1) << std::endl;
  }
}

void LuaRunner::loadFunction(std::string name, lua_CFunction fn)
{
  std::lock_guard<std::mutex> lock(mutex);

  lua_pushcfunction(state, fn);
  lua_setglobal(state, name.c_str());
}

void LuaRunner::callFunction(std::string name, std::vector<Param>& params)
{
  std::lock_guard<std::mutex> lock(mutex);

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
  std::lock_guard<std::mutex> lock(mutex);

  lua_getglobal(state, name.c_str());
  if (lua_isfunction(state, -1)) {
    if (lua_pcall(state, 0, 0, 0) != 0) {
      std::cerr << "Lua error: " << lua_tostring(state, -1) << std::endl;
    }
  }
}

void LuaRunner::setTable(std::string name, std::vector<float>& buff)
{
  std::lock_guard<std::mutex> lock(mutex);

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

void LuaRunner::setDirectPanelControl() {
  if (panel) {
    auto cb = [this](std::string device, int pin, int value)
    {
      std::lock_guard<std::mutex> lock(mutex);

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
    panel->registerCallback(DIRECT_CONTROL, cb);
  }
}