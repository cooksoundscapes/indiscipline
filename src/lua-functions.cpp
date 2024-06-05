#include "lua-runner.h"
#include "cairo-wrapper.h"
#include <iostream>
#include <bitset>
#include <lauxlib.h>
#include <lua.h>

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

int _set_source_rgba(lua_State* l) { 
  lua_check_num_args(l, 4);
  double r = luaL_checknumber(l, 1);
  double g = luaL_checknumber(l, 2);
  double b = luaL_checknumber(l, 3);
  double a = luaL_checknumber(l, 4);
  Cairo::set_source_rgba(r, g, b, a);
  lua_settop(l, 0);
  return 0;
}

int _new_path(lua_State* l) {
  Cairo::new_path();
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

int _set_operator(lua_State* l) {
  lua_check_num_args(l, 1);
  const char* op = luaL_checkstring(l, 1);
  Cairo::set_operator(op);
  lua_settop(l, 0);
  return 0;
}

int _text(lua_State* l) {
  Cairo::TextParams params;

  const char* txt = luaL_checkstring(l, 1);
  double size = luaL_optnumber(l, 2, 0);
  const char* font = luaL_optstring(l, 3, "");
  double width = luaL_optnumber(l, 4, 0);
  const char* alignment = luaL_optstring(l, 5, "");
  bool centered = luaL_optnumber(l, 6, 0);

  lua_getglobal(l, "FontSize");
  if (lua_isnumber(l, -1) && size == 0) {
    size = lua_tonumber(l, -1);
  }
  bool enableAntiAlias = false;
  lua_getglobal(l, "FontAntiAlias");
  if (lua_isboolean(l, -1)) {
    enableAntiAlias = lua_toboolean(l, -1);
  }
  lua_getglobal(l, "DefaultFont");
  if (font[0] == '\0') {
    if (lua_isstring(l, -1))
      font = lua_tostring(l, -1);
    else
      font = "Sans";
  }

  if (strlen(alignment) != 0) {
    params.alignment = alignment;
  }
  params.text = txt;
  params.size = size;
  params.font = font;
  params.width = width;
  params.centered = centered;
  params.enableAntiAlias = enableAntiAlias;

  Cairo::text(params);
  lua_settop(l, 0);
  return 0;
}

int _create_surface(lua_State* l) {
  lua_check_num_args(l, 3);
  const char* name = luaL_checkstring(l, 1);
  double width = luaL_checknumber(l, 2);
  double height = luaL_checknumber(l, 3);

  Cairo::create_additional_surface(name, width, height);
  lua_settop(l, 0);
  return 0;
}
int _draw_surface(lua_State* l) {
  lua_check_num_args(l, 3);
  const char* name = luaL_checkstring(l, 1);
  double x = luaL_checknumber(l, 2);
  double y = luaL_checknumber(l, 3);

  Cairo::draw_surface(name, x, y);
  lua_settop(l, 0);
  return 0;
}

int _destroy_surface(lua_State* l) {
  lua_check_num_args(l, 1);
  const char* name = luaL_checkstring(l, 1);
  
  Cairo::destroy_surface(name);
  return 0;
}

int _set_line_cap(lua_State* l) {
  lua_check_num_args(l, 1);
  const char* type = luaL_checkstring(l, 1);

  Cairo::set_line_cap(type);
  return 0;
}

int _hex_to_rgb(lua_State* l) {
  lua_check_num_args(l, 1);
  const char* hex_v = luaL_checkstring(l, 1);
  int red{0}, green{0}, blue{0};
  std::sscanf(hex_v, "#%02x%02x%02x", &red, &green, &blue);
  lua_pushnumber(l, red / 255.0);
  lua_pushnumber(l, green / 255.0);
  lua_pushnumber(l, blue / 255.0);

  return 3;
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

int LuaRunner::getBufferSize(lua_State* l) {
  lua_check_num_args(l, 1);
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, 1));

  int b_size = luaRunner->audioSink->getBufferSize();
  lua_pushnumber(l, b_size);
  return 1;
}

int LuaRunner::startJack(lua_State* l) {
  lua_check_num_args(l, 1);
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, 1));
  luaRunner->audioSink->start();
  return 0;
}

int LuaRunner::stopJack(lua_State* l) {
  lua_check_num_args(l, 1);
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, 1));
  luaRunner->audioSink->stop();
  return 0;
}

int LuaRunner::setPanelLights(lua_State* l) {
  lua_check_num_args(l, 2);
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, 1));
  int state = luaL_checknumber(l, 2);
  std::bitset<8> bitState(state);
  luaRunner->panel->setDeviceState(LED_ARRAY, bitState);
  return 0;

}

int LuaRunner::loadModule(lua_State* l) {
  lua_check_num_args(l, 2);
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, 1));
  const char* file = luaL_checkstring(l, 2);

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
  lua_check_num_args(l, 2);
  auto luaRunner = reinterpret_cast<LuaRunner*>(lua_touserdata(l, 1));
  const char* ip_addr = luaL_checkstring(l, 2);

  luaRunner->setIPTarget(ip_addr);
  return 0;
}
