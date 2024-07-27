#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_events.h>
#include <vector>
#include <memory>
#include "screen-base.h"

class Window : public ScreenBase {
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  TTF_Font* font = NULL;
  SDL_Texture* screen = NULL;

  void updateWindow();
  void handleEvents();
  void handleKeyboardEvent(SDL_Event&);
  void draw();

  struct Component {
    SDL_Rect rect;
    SDL_Texture* texture;
    int luaTableRef;
  };

  std::vector<Component> components;
  std::vector<unsigned int> dirtyTexturesIds; 

  int addComponent(int x, int y, int w, int h, int luaRef) {
    SDL_Texture* new_t = NULL;
    components.push_back({{x, y, w, h}, new_t, luaRef});
    return components.size() - 1;
  }

  Uint32 frameDuration;
  bool shouldPrint = false;

public: 
  Window(int w, int h);
  ~Window();

  void setSize(int w, int h);

  void setFps(int fps) override {
    frameDuration = 1000 / fps;
  }

  void setLuaInterpreter(std::shared_ptr<LuaRunnerBase> LIntr) override {
    this->luaInterpreter = LIntr;
    LIntr->setGlobal("Window", this);
    LIntr->loadFunction("add_component", &Window::_addComponent);
  }

  void loadFile(const char*) override;

  void schedulePrint() override {
    shouldPrint = true;
  }

  void loop();

  // functions to be registered at lua State
  static int _addComponent(lua_State*);
};