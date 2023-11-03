#include <lo/lo.h>
#include <memory>
#include "lua-runner-abstract.h"

class OscServer {
  lo_server_thread thread;
  std::shared_ptr<AbstractLuaRunner> luaRunner;

public:
  void setLuaInterpreter(std::shared_ptr<AbstractLuaRunner> luaR) { this->luaRunner = luaR; }
  void init();
};