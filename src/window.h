#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_events.h>
#include <vector>
#include <memory>
#include <mutex>
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
  };

  std::vector<Component> components;
  std::vector<unsigned int> dirtyTexturesIds;
  std::recursive_mutex redrawMutex;

  int addComponent(int x, int y, int w, int h, int luaRef);
  void prepareComponentTexture(Component& component);
  void finishComponentDraw(Component& component);

  void addDirtyTexture(int textureIndex)
  {
    std::lock_guard<std::recursive_mutex> lock(redrawMutex);
    if (textureIndex < components.size()) {
     dirtyTexturesIds.push_back(textureIndex);
    }
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
    LIntr->loadFunction("mark_dirty", &Window::_markDirty);
  }

  void loadFile(const char*) override;

  void schedulePrint() override {
    shouldPrint = true;
  }

  void loop();

  // functions to be registered at lua State
  static int _addComponent(lua_State*);
  static int _markDirty(lua_State*);
};