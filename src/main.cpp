#include "window.h"
#include "lua-runner.h"
#include "audio-sink.h"
#include "osc-server.h"
#ifdef USE_GPIO
  #include "hardware/panel.h"
#endif
#include <iostream>
#include <filesystem>
#include <string>

constexpr int WIDTH = 320;
constexpr int HEIGHT = 240;

int main(int argc, const char** argv) {
  if (argc < 2) {
    std::cerr << "Provide a lua script\n";
    exit(1);
  }
  std::string filename = argv[1];
  std::filesystem::path currentPath = std::filesystem::current_path();
  std::string scriptsDir = currentPath.parent_path().string() + "/scripts/";
  std::cout << scriptsDir << '\n';
  //-----------------------------//

  Window window(WIDTH, HEIGHT);
  OscServer oscServer;

  auto luaInterpreter = std::make_shared<LuaRunner>();
  auto audioSink = std::make_shared<AudioSink>(10);
  #ifdef USE_GPIO
    auto panel = std::make_shared<Panel>();
    window.setHardwarePanel(panel);
  #endif

  luaInterpreter->setAudioSink(audioSink);  

  window.setLuaInterpreter(luaInterpreter);
  oscServer.setLuaInterpreter(luaInterpreter);

  luaInterpreter->setGlobal("screen_w", WIDTH);
  luaInterpreter->setGlobal("screen_h", HEIGHT);

  window.loadLuaScript(scriptsDir + filename);

  oscServer.init();
  window.loop();
}