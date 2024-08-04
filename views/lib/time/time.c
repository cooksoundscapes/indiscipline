#include <unistd.h>
#include <time.h>
#include "lua.h"
#include "lauxlib.h"

int time_ms(lua_State* l) {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  int secs_in_millis = t.tv_sec * 1000;
  int nanos_in_millis = t.tv_nsec / 1000000;
  lua_pushnumber(l, secs_in_millis + nanos_in_millis);
  return 1;
}

int time_sleep (lua_State *L) {
  int msec = (int) luaL_checknumber(L, -1);
  lua_pop (L, 1);
  usleep (msec * 1000);
  return 0;
}

// estranho - pd_lua pede por "luaopen_time",
// enquanto o indiiscipline pede por luaopen_lib_time!
int luaopen_lib_time(lua_State* l) {
  lua_newtable(l);
  lua_pushcfunction(l, time_ms);
  lua_setfield(l, -2, "ms");
  lua_pushcfunction(l, time_sleep);
  lua_setfield(l, -2, "sleep");
  return 1;
}