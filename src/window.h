#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_events.h>
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>
#include "screen-base.h"

class Window : public ScreenBase {
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  TTF_Font* font = NULL;
  SDL_Texture* screen = NULL;

  void updateWindow();
  void handleEvents();
  void handleKeyboardEvent(SDL_Event&);

  struct Component {
    SDL_Rect rect;
    SDL_Texture* texture;
    int luaTableRef;
    uint surfaceId;
  };

  std::vector<Component> components;
  std::vector<unsigned int> dirtyTexturesIds;
  std::vector<Component> liveComponents;

  int addComponent(int x, int y, int w, int h, int luaRef);
  void addLiveComponent(int x, int y, int w, int h, int luaRef);  
  void drawComponent(uint textureId);
  void drawLiveComponent(Component&);

  void addDirtyTexture(int textureIndex)
  {
    if (textureIndex < components.size()) {
      auto it = std::find(dirtyTexturesIds.begin(), dirtyTexturesIds.end(), textureIndex);
      if (it == dirtyTexturesIds.end()) {
        dirtyTexturesIds.push_back(textureIndex);
      }
    }
  }

  Uint32 frameDuration;

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
    LIntr->loadFunction("add_live_component", &Window::_addLiveComponent);
  }

  void loadFile(const char*) override;

  void loop();

  // functions to be registered at lua State
  static int _addComponent(lua_State*);
  static int _addLiveComponent(lua_State*);
};