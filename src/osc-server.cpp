#include "osc-server.h"
#include <iostream>
#include <vector>
#include <string>

static void error_handler(int num, const char *msg, const char *path) {
    std::cout << "liblo server error " << num << " in path " << path << ": " << msg << '\n';
}

static int 
param_handler(const char* p, const char* types, lo_arg** argv, int argc, lo_message data, void* userData)
{
  auto luaRunner = (LuaRunnerBase*) userData;
  std::vector<LuaRunnerBase::Param> params = {};

  for (int i = 0; types[i] != '\0'; i++) {
    char t = types[i];
    if (t == 'f')
      params.push_back({t, argv[i]->f, ""});
    else if (t == 's') 
      params.push_back({t, 0, &argv[i]->s});
  }
  luaRunner->callFunction("SetParam", params);
  return 0;
}

void OscServer::init() {
  if (!luaRunner) {
    std::cerr << "[OscServer] Error: missing Lua Interpreter;\n";
    return;
  }
  thread = lo_server_thread_new("7777", error_handler);

  // add methods
  lo_server_thread_add_method(thread, "/param", NULL, param_handler, luaRunner.get());

  // server start
  lo_server_thread_start(thread);
  std::cout << "Listening to port 7777" << '\n';
}