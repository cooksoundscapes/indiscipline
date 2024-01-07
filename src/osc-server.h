#include <lo/lo.h>
#include <memory>
#include "lua-runner-base.h"

class OscServer {
  lo_server_thread thread;
  std::shared_ptr<LuaRunnerBase> luaRunner;

public:
  void setLuaInterpreter(std::shared_ptr<LuaRunnerBase> luaR) { this->luaRunner = luaR; }
  void init();

  ~OscServer() {
    lo_server_thread_free(thread);
  }
};
