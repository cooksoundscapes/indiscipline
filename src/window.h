#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_events.h>
#include "lua-runner-base.h"
#include <memory>

class Window {
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  int w, h;
  bool shouldQuit = false;
  long fps = 0;
  TTF_Font* font = NULL;
  SDL_Event event_handler;
  SDL_Texture* screen = NULL;

  void updateWindow();
  void handleEvents();
  void draw();

  std::shared_ptr<LuaRunnerBase> luaInterpreter;

public: 
  Window(int w, int h);
  ~Window();

  void loop();
  void stop() {}

  void setLuaInterpreter(std::shared_ptr<LuaRunnerBase> LIntr) {
    this->luaInterpreter = LIntr;
  }

  void loadLuaScript(std::string file);
};