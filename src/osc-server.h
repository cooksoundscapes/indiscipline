#include <lo/lo.h>
#include <memory>
#include "screen-base.h"

class OscServer {
  lo_server_thread thread;
  std::shared_ptr<ScreenBase> window;

public:
  void setWindow(std::shared_ptr<ScreenBase> luaR) { this->window = luaR; }
  void init();

  ~OscServer() {
    lo_server_thread_free(thread);
  }
};