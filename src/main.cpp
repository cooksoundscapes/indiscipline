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

int main(int argc, const char** argv) {
  // retrieve args "-w" and "-h" and their values,
  // also check if they are valid
  int width = 0;
  int height = 0;
  for (int i = 0; i < argc; i++) {
    if (std::string(argv[i]) == "-w") {
      // avoid segfault if no value is provided
      if (i+1 >= argc) {
        std::cout << "Invalid width value\n";
        return 1;
      }
      width = std::stoi(argv[i+1]);
    }
    if (std::string(argv[i]) == "-h") {
      // avoid segfault if no value is provided
      if (i+1 >= argc) {
        std::cout << "Invalid height value\n";
        return 1;
      }
      height = std::stoi(argv[i+1]);
    }
  }

  // initialize osc server, GPIOs and hardware panel
  OscServer oscServer;

  // setup shared objects for lua and audio
  auto luaInterpreter = std::make_shared<LuaRunner>();
  auto audioSink = std::make_shared<AudioSink>(A_CHANNELS);
  luaInterpreter->setAudioSink(audioSink);

  #ifdef USE_GPIO
    auto gpio = std::make_shared<GPIO>();
    auto panel = std::make_shared<Panel>(gpio);
    luaInterpreter->setPanel(panel);



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
  graphics.loop();
  
  return 0;
}
