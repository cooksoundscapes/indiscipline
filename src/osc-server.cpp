#include "osc-server.h"
#include "main.h"
#include <iostream>
#include <vector>
#include <string>

static void error_handler(int num, const char *msg, const char *path) {
    std::cout << "OSC server error " << num << " in path " << path << ": " << msg << '\n';
}

static int 
navigate_handler(const char* p, const char* types, lo_arg** argv, int argc, lo_message data, void* userData)
{
  auto luaRunner = (LuaRunnerBase*) userData;
  std::vector<LuaRunnerBase::Param> params = {};

  auto target = &argv[0]->s;
  luaRunner->loadFile(target);
  return 0;
}

static int 
direct_input_handler(const char* p, const char* types, lo_arg** argv, int argc, lo_message data, void* userData)
{
  auto luaRunner = (LuaRunnerBase*) userData;
  std::vector<LuaRunnerBase::Param> params = {};

  const char* device = &argv[0]->s;
  float pin = argv[1]->f;
  float value = argv[2]->f;
  params.push_back({'s', 0, device});
  params.push_back({'f', pin, ""});
  params.push_back({'f', value, ""});

  luaRunner->callFunction(PANEL_INPUT, params);
  return 0;
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
  luaRunner->callFunction(SET_PARAM, params);
  return 0;
}

static int 
buffer_handler(const char* p, const char* types, lo_arg** argv, int argc, lo_message data, void* userData)
{
  auto luaRunner = (LuaRunnerBase*) userData;
  std::vector<float> buffer;
  std::string target;

  for (int i = 0; types[i] != '\0'; i++) {
    if (i == 0 && types[i] != 's') {
      std::cerr << "Error: " << p << " requires 1st arg to be of type string\n";
      return 0;
    } else if (i != 0 && types[i] != 'f') {
      std::cerr << "Error: " << p << " requires a float array\n";
      return 0;
    } else if (types[i] == 's') {
      target = &argv[i]->s;
    } else if (types[i] == 'f') {
      buffer.push_back(argv[i]->f);
    }
  }
  luaRunner->setTable(target, buffer);
  return 0;
}

void OscServer::init() {
  if (!luaRunner) {
    std::cerr << "[OscServer] Error: missing Lua Interpreter;\n";
    return;
  }
  thread = lo_server_thread_new(OSC_SERV, error_handler);

  // add methods
  lo_server_thread_add_method(thread, "/navigate", "s", navigate_handler, luaRunner.get());
  lo_server_thread_add_method(thread, "/param", NULL, param_handler, luaRunner.get());
  lo_server_thread_add_method(thread, "/buffer", NULL, buffer_handler, luaRunner.get());
  lo_server_thread_add_method(thread, "/panel", "sff", buffer_handler, luaRunner.get());

  // server start
  lo_server_thread_start(thread);
  std::cout << "Listening to port " << OSC_SERV << '\n';
}