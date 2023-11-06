#include "window.h"
#include "lua-runner.h"
#include "audio-sink.h"
#include "osc-server.h"
#include "display.h"
#include "hardware/panel.h"
#include "hardware/ssd1306-spi.h"
#include <iostream>
#include <filesystem>
#include <string>

constexpr int WINDOW_WIDTH = 320;
constexpr int WINDOW_HEIGHT = 240;
constexpr int OLED_DISPLAY_WIDTH = 128;
constexpr int OLED_DISPLAY_HEIGHT = 64;


int main(int argc, const char** argv) {
	// setup hardcoded lua scripts path
  if (argc < 2) {
    std::cerr << "Provide a lua script\n";
    exit(1);
  }
  std::string filename = argv[1];
  std::filesystem::path currentPath = std::filesystem::current_path();
  std::string scriptsDir = currentPath.parent_path().string() + "/scripts/";
  std::cout << scriptsDir << '\n';

	// define main graphics driver
  //Window window(WINDOW_WIDTH, WINDOW_HEIGHT);
  Display display(OLED_DISPLAY_WIDTH, OLED_DISPLAY_HEIGHT);
  auto displayDevice = std::make_shared<SSD1306_SPI>();
  display.setDevice(displayDevice);
  
  // initialize hardware panel and osc server
  Panel panel;
  OscServer oscServer;
  
  // setup shared objects for lua and audio
  auto luaInterpreter = std::make_shared<LuaRunner>();
  auto audioSink = std::make_shared<AudioSink>(10);
  luaInterpreter->setAudioSink(audioSink);
  luaInterpreter->setGlobal("screen_w", OLED_DISPLAY_WIDTH);
  luaInterpreter->setGlobal("screen_h", OLED_DISPLAY_HEIGHT);
  
  // setup lua interpreter at graphics driver and OSC
	//window.setLuaInterpreter(luaInterpreter);
  display.setLuaInterpreter(luaInterpreter);
  oscServer.setLuaInterpreter(luaInterpreter);

	// load lua script from args (TEMPORARY)
  //window.loadLuaScript(scriptsDir + filename);
  display.loadLuaScript(scriptsDir + filename);

	// start osc server and main loop
  oscServer.init();
  //window.loop();
  display.loop();
}
