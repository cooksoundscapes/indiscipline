#include "lua-runner.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "cairo-wrapper.h"

int lua_check_num_args(lua_State* l, int n) {
  if (lua_gettop(l) != n) {
    return luaL_error(l, "error: wrong number of arguments;");
  }
  return 0;
}

int _set_source_rgb(lua_State* l) { 
  lua_check_num_args(l, 3);
  double r = luaL_checknumber(l, 1);
  double g = luaL_checknumber(l, 2);
  double b = luaL_checknumber(l, 3);
  Cairo::set_source_rgb(r, g, b);
  lua_settop(l, 0);
  return 0;
}

int _rectangle(lua_State* l) {
  lua_check_num_args(l, 4);
  double x = luaL_checknumber(l, 1);
  double y = luaL_checknumber(l, 2);
  double w = luaL_checknumber(l, 3);
  double h = luaL_checknumber(l, 4);
  Cairo::rectangle(x, y, w, h);
  lua_settop(l, 0);
  return 0;
}

int _arc(lua_State* l) {
  lua_check_num_args(l, 5);
  double xc = luaL_checknumber(l, 1);
  double yc = luaL_checknumber(l, 2);
  double rad = luaL_checknumber(l, 3);
  double ang1 = luaL_checknumber(l, 4);
  double ang2 = luaL_checknumber(l, 5);
  Cairo::arc(xc, yc, rad, ang1, ang2);
  lua_settop(l, 0);
  return 0;
}

int _move_to(lua_State* l) {
  lua_check_num_args(l, 2);
  double x = luaL_checknumber(l, 1);
  double y = luaL_checknumber(l, 2);
  Cairo::move_to(x, y);
  lua_settop(l, 0);
  return 0;
}

int _line_to(lua_State* l) {
  lua_check_num_args(l, 2);
  double x = luaL_checknumber(l, 1);
  double y = luaL_checknumber(l, 2);
  Cairo::line_to(x, y);
  lua_settop(l, 0);
  return 0;
}

int _rel_move_to(lua_State* l) {
  lua_check_num_args(l, 2);
  double x = luaL_checknumber(l, 1);
  double y = luaL_checknumber(l, 2);
  Cairo::rel_move_to(x, y);
  lua_settop(l, 0);
  return 0;
}

int _rel_line_to(lua_State* l) {
  lua_check_num_args(l, 2);
  double x = luaL_checknumber(l, 1);
  double y = luaL_checknumber(l, 2);
  Cairo::rel_line_to(x, y);
  lua_settop(l, 0);
  return 0;
}

int _set_line_width(lua_State* l) {
  lua_check_num_args(l, 1);
  double w = luaL_checknumber(l, 1);
  Cairo::set_line_width(w);
  lua_settop(l, 0);
  return 0;
}

int _paint(lua_State* l) {
  Cairo::paint();
  lua_settop(l, 0);
  return 0;
}

int _fill(lua_State* l) {
  Cairo::fill();
  lua_settop(l, 0);
  return 0;
}

int _stroke(lua_State* l) {
  Cairo::stroke();
  lua_settop(l, 0);
  return 0;
}

int _text(lua_State* l) {
  lua_check_num_args(l, 4);
  const char* txt = luaL_checkstring(l, 1);
  double size = luaL_checknumber(l, 2);
  double width = luaL_checknumber(l, 3);
  bool centered = luaL_checknumber(l, 4);
  Cairo::text(txt, size, width, centered);
  lua_settop(l, 0);
  return 0;
}

// retrieve audio buffer function from sink
int LuaRunner::getAudioBuffer(lua_State* l) {
  lua_check_num_args(l, 2);
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, 1));
  double channel = luaL_checknumber(l, 2);
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

int hexToRGB(lua_State* l) {
  lua_check_num_args(l, 1);
  const char* hex_v = luaL_checkstring(l, 1);
  int red{0}, green{0}, blue{0};
  std::sscanf(hex_v, "#%02x%02x%02x", &red, &green, &blue);
  lua_pushnumber(l, red / 255.0);
  lua_pushnumber(l, green / 255.0);
  lua_pushnumber(l, blue / 255.0);

  return 3;
}

LuaRunner::LuaRunner() {
  state = luaL_newstate();
  luaL_openlibs(state);
  std::cout << "Initialized Lua API - version " << lua_version(NULL) << std::endl;

  // expose lua interpreter class to the scripts
  lua_pushlightuserdata(state, this);
  lua_setglobal(state, "caller");

  loadFunction("set_source_rgb", &_set_source_rgb);
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
  loadFunction("get_audio_buffer", &LuaRunner::getAudioBuffer);
  loadFunction("hex", &hexToRGB);
}

LuaRunner::~LuaRunner() {
  lua_close(state);
}

void LuaRunner::loadFile(std::string file)
{
  if (luaL_dofile(state, file.c_str()) == 0) {
    std::cout << "Successfully loaded " << file << std::endl;
  } else {
    std::cerr << "Failed to load script " << file << ": " << lua_tostring(state, -1) << std::endl;
  }
}

void LuaRunner::setGlobal(std::string varname, double value) {
  lua_pushnumber(state, value);
  lua_setglobal(state, varname.c_str());
}

void LuaRunner::draw() {
  lua_getglobal(state, "Draw");

  if (lua_pcall(state, 0, 0, 0) != 0) {
    std::cerr << "Lua error: " << lua_tostring(state, -1) << std::endl;
  }
}

void LuaRunner::loadFunction(std::string name, lua_CFunction fn)
{
  lua_pushcfunction(state, fn);
  lua_setglobal(state, name.c_str());
}

void LuaRunner::callFunction(std::string name, std::vector<Param> params) {
  int argcount = params.size();
  
  lua_getglobal(state, name.c_str());

  if (lua_isfunction(state, -1)) {
    // push arguments
    for (const auto& p : params) {
      if (p.type == 'f')
        lua_pushnumber(state, p.f_value);
      else if (p.type == 's')
        lua_pushstring(state, p.s_value); 
    }
    if (lua_pcall(state, argcount, 0, 0) != 0) {
      std::cerr << "Lua error: " << lua_tostring(state, -1) << std::endl;
    }
  }
}