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

void setup(
  int& w,
  int& h,
  int& targetFps,
  std::string& luaPath,
  std::string& ipTarget,
  int& audio_channels,
  std::string& default_view
) {
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
  targetFps = TARGET_FPS;
  luaPath = home + "/views/";
  audio_channels = A_CHANNELS;
  default_view = HOME_PAGE;

  std::string configPath = home + "/.bouncersettings";
  std::ifstream handler(configPath);

  if (handler.is_open()) {
    std::string line;
    while (std::getline(handler, line)) {
      auto s = line.find("=");
      std::string key = line.substr(0U, s);
      std::string value = line.substr(s+1, std::string::npos);
      key.erase(key.find_last_not_of(' ')+1);
      key.erase(0, key.find_first_not_of(' '));
      value.erase(value.find_last_not_of(' ')+1);
        value.erase(0, value.find_first_not_of(' '));
      if (key == "width") {
        w = std::stoi(value);
      } else if (key == "height") {
        h = std::stoi(value);
      } else if (key == "fps") {
        targetFps = std::stoi(value);
      } else if (key == "lua-path") {
        if (value[0] == '~') {
          value.erase(0,1);
          value = home + value;
        }
        if (value[value.length()-1] != '/') value += '/';
        luaPath = value;
      } else if (key == "ip-target") {
        ipTarget = value;
      } else if (key == "audio-channels") {
        audio_channels = std::stoi(value);
      } else if (key == "default-view") {
        default_view = value;
      }
    }
  }
}

int main()
{
  int width, height, audioChannels, targetFps;
  std::string luaPath, ipTarget, defaultView;
  bool allowResize;
  setup(width, height, targetFps, luaPath, ipTarget, audioChannels, defaultView);
  std::cout << "Rendering screen at " << width << 'x' << height << " at " << targetFps << "FPS;\nLua path is " << luaPath << ";\n";

  graphics.setFps(targetFps);

  OscServer oscServer;
  auto luaInterpreter = std::make_shared<LuaRunner>(width, height, luaPath, ipTarget, defaultView);
  auto audioSink = std::make_shared<AudioSink>(audioChannels);

  luaInterpreter->setAudioSink(audioSink);

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

  // setup lua interpreter at graphics driver and OSC  
  graphics.setLuaInterpreter(luaInterpreter);
  oscServer.setLuaInterpreter(luaInterpreter);

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

  luaInterpreter->init();

  graphics.loop();
  
  return 0;
}
