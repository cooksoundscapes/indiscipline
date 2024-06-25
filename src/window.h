#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_events.h>
#include "screen-base.h"
#include <memory>

class Window : public ScreenBase {
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  TTF_Font* font = NULL;
  SDL_Texture* screen = NULL;

  void updateWindow();
  void handleEvents();
  void handleKeyboardEvent(SDL_Event&);
  void draw();

  Uint32 frameDuration;

public: 
  Window(int w, int h);
  ~Window();

  void setSize(int w, int h);

  void setFps(int fps) override {
    frameDuration = 1000 / fps;
  }

  void loop();
};