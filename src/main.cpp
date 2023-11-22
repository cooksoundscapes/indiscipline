#include "main.h"
#include "window.h"
#include "lua-runner.h"
#include "audio-sink.h"
#include "osc-server.h"
#include "display.h"
#include "hardware/panel.h"
#include "hardware/ssd1306-spi.h"
#include "hardware/gpio.h"
#include <iostream>
#include <string>
#include <csignal>
#include <lo/lo.h>
#include <functional>
#include <memory>



// define main graphics driver globally
//Window window(WINDOW_WIDTH, WINDOW_HEIGHT);
Display display(OLED_DISPLAY_WIDTH, OLED_DISPLAY_HEIGHT);

void signalHandler(int signal) {
  if (signal == SIGINT) {
    display.stop();
  }
}

int main() {
  // initialize GPIOs, hardware panel and osc server
  auto gpio = std::make_shared<GPIO>();
  auto panel = std::make_shared<Panel>(gpio);
  OscServer oscServer;


  // setup function for send OSC
  lo_address osc_addr = lo_address_new(NULL, OSC_CLIENT);
  auto sendOsc = [&osc_addr](std::string device, int pin, int value){
    auto path = "/" + device + "/" + std::to_string(pin);
	  lo_send(osc_addr, path.c_str(), "f", (float)value);
  };
  panel->registerCallback(SEND_OSC, sendOsc);


  // setup display device
  auto displayDevice = std::make_shared<SSD1306_SPI>(gpio);
  display.setDevice(displayDevice);
  

  // setup shared objects for lua and audio
  auto luaInterpreter = std::make_shared<LuaRunner>();
  auto audioSink = std::make_shared<AudioSink>(A_CHANNELS);
  luaInterpreter->setAudioSink(audioSink);
  luaInterpreter->setPanel(panel);
  luaInterpreter->setGlobal(SCREEN_W, OLED_DISPLAY_WIDTH);
  luaInterpreter->setGlobal(SCREEN_H, OLED_DISPLAY_HEIGHT);


  // setup lua interpreter at graphics driver and OSC
	//window.setLuaInterpreter(luaInterpreter);
  display.setLuaInterpreter(luaInterpreter);
  oscServer.setLuaInterpreter(luaInterpreter);


  //window.loadLuaScript(HOME_PAGE);
  display.loadLuaScript(HOME_PAGE);

  signal(SIGINT, signalHandler);
	// start osc server and main loop
  oscServer.init();
  //window.loop();
  display.loop();
  
  return 0;
}
