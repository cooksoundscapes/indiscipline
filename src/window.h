#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_events.h>
#include "screen-base.h"
#include <memory>

class Window : public ScreenBase {
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;

  long fps = 0;
  TTF_Font* font = NULL;
  SDL_Event event_handler;
  SDL_Texture* screen = NULL;

  void updateWindow();
  void handleEvents();
  void handleKeyboardEvent();
  void draw();

  bool resize = false;  

public: 
  Window(int w, int h);
  ~Window();

  void setSize(int w, int h);

  void loop();

  void allowResize() override {
    resize = true;
  }
};