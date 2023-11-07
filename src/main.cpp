#include "window.h"
#include "lua-runner.h"
#include "audio-sink.h"
#include "osc-server.h"
#include "display.h"
#include "hardware/panel.h"
#include "hardware/ssd1306-spi.h"
#include "hardware/gpio.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <csignal>

constexpr int WINDOW_WIDTH = 320;
constexpr int WINDOW_HEIGHT = 240;
constexpr int OLED_DISPLAY_WIDTH = 128;
constexpr int OLED_DISPLAY_HEIGHT = 64;

// define main graphics driver globally
//Window window(WINDOW_WIDTH, WINDOW_HEIGHT);
Display display(OLED_DISPLAY_WIDTH, OLED_DISPLAY_HEIGHT);

void signalHandler(int signal) {
  if (signal == SIGINT) {
    display.stop();
  }
}

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
  
  // initialize GPIOs, hardware panel and osc server
  auto gpio = std::make_shared<GPIO>();
  Panel panel(gpio);
  OscServer oscServer;

  // setup display device
  auto displayDevice = std::make_shared<SSD1306_SPI>(gpio);
  display.setDevice(displayDevice);
  
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

  signal(SIGINT, signalHandler);

	// start osc server and main loop
  oscServer.init();
  //window.loop();
  display.loop();
  
  return 0;
}
