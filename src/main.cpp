#include "main.h"
#ifdef USE_SSD1306
  #include "hardware/ssd1306-spi.h"
  #include "spi-display.h"
#else
  #ifdef USE_FB
    #include "fb-display.h"
  #else
    #include "window.h"
  #endif
#endif
#ifdef USE_GPIO
  #include "hardware/gpio.h"
  #include "hardware/panel.h"
#endif
#include "lua-runner.h"
#include "audio-sink.h"
#include "osc-server.h"
#include <iostream>
#include <string>
#include <csignal>
#include <lo/lo.h>
#include <functional>
#include <memory>
#include <fstream>

// define main graphics driver globally
#ifdef USE_SSD1306
  SPIDisplay graphics(OLED_DISPLAY_WIDTH, OLED_DISPLAY_HEIGHT);
#else
  #ifdef USE_FB
    FramebufferDisplay graphics(WINDOW_WIDTH, WINDOW_HEIGHT);
  #else
    Window graphics(WINDOW_WIDTH, WINDOW_HEIGHT);
  #endif
#endif

void signalHandler(int signal) {
  if (signal == SIGINT) {
    graphics.stop();
  }
}

void setup(int& w, int& h, std::string& luapath, std::string& iptarget)
{
  std::string home = getenv("HOME");
  if (home.empty()) {
  	std::cerr << "Invalid $HOME env variable, exiting.\n";
  	exit(1);
  }
  #ifdef USE_SSD1306
    w = OLED_DISPLAY_WIDTH;
    h = OLED_DISPLAY_HEIGHT;
  #else
    w = WINDOW_WIDTH;
    h = WINDOW_HEIGHT;
  #endif

  luapath = home + "/views";
  std::string configPath = home + "/.bouncersettings";

  if (handler.is_open()) {
    while (std::getline(handler, line)) {
      auto s = line.find("=");
      std::string key = line.substr(0U, s);
      std::string value = line.substr(s+1, std::string::npos);
      key.erase(key.find_last_not_of(' ')+1);
      key.erase(0, key.find_first_not_of(' '));
      value.erase(value.find_last_not_of(' ')+1);
        value.erase(0, value.find_first_not_of(' '));
      if (key == "width") {
        width = std::stoi(value);
      } else if (key == "height") {
        height = std::stoi(value);
      } else if (key == "lua-path") {
        if (value[0] == '~') {
          value.erase(0,1);
          value = home + value;
        }
        luaPath = value + '/';
      } else if (key == "ip-target") {
        iptarget = value;
      }
    }
  }
}

int main()
{
  int width, height;
  std::string luaPath, ipTarget;
  setup(width, height, luaPath, ipTarget);

  std::cout << "Rendering screen at " << width << 'x' << height << ";\nLua path is " << luaPath << ";\n";

  OscServer oscServer;
  auto luaInterpreter = std::make_shared<LuaRunner>();
  auto audioSink = std::make_shared<AudioSink>(A_CHANNELS);
  luaInterpreter->setAudioSink(audioSink);
  luaInterpreter->setProjectPath(luaPath);
  if (!ipTarget.empty()) {
    luaInterpreter->setIPTarget(ipTarget);
  }

  #ifdef USE_GPIO
    auto gpio = std::make_shared<GPIO>();
    auto panel = std::make_shared<Panel>(gpio);
    luaInterpreter->setPanel(panel);
    std::cout << "GPIO usage enabled;\n";

    #ifdef USE_SSD1306
      // setup display device
      auto displayDevice = std::make_shared<SSD1306_SPI>(gpio);
      graphics.setDevice(displayDevice);
    #endif
  #endif

  if (width > 0 && height > 0) {
    graphics.setSize(width, height);
  }
  #ifdef USE_SSD1306
    luaInterpreter->setGlobal(SCREEN_W, (width > 0) ? width : OLED_DISPLAY_WIDTH);
    luaInterpreter->setGlobal(SCREEN_H, (height > 0) ? height : OLED_DISPLAY_HEIGHT);
  #else
    luaInterpreter->setGlobal(SCREEN_W, (width > 0) ? width : WINDOW_WIDTH);
    luaInterpreter->setGlobal(SCREEN_H, (height > 0) ? height : WINDOW_HEIGHT);
  #endif

  // setup lua interpreter at graphics driver and OSC  
  graphics.setLuaInterpreter(luaInterpreter);
  oscServer.setLuaInterpreter(luaInterpreter);

  graphics.loadLuaScript(HOME_PAGE);

  signal(SIGINT, signalHandler);

	// start osc server and main loop
  oscServer.init();
  #ifdef USE_FB
    std::cout << "Using /dev/fb0 as graphics output;\n";
  #else
    #ifdef USE_SSD1306
      std::cout << "Using /dev/spidev0.0 as graphics output (SSD1306);\n";
    #else
      std::cout << "Using SDL2 as graphics output;\n";
    #endif
  #endif

  graphics.loop();
  
  return 0;
}
