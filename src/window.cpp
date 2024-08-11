#include "main.h"
#include "window.h"
#include "cairo-wrapper.h"
#include <iostream>
#include <SDL2/SDL_image.h>

Window::Window(int w, int h) : ScreenBase(w, h)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << SDL_GetError() << '\n';
    exit(1);
  }
  window = SDL_CreateWindow(
    "Indiscipline", 
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    w,
    h,
    SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE
  );
  if (window == NULL) {
    std::cerr << SDL_GetError() << '\n';
    exit(1);
  }
  renderer = SDL_CreateRenderer(window, -1, 
    SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED
  ); 
  if (renderer == NULL) {
    std::cerr << SDL_GetError() << '\n';  
    exit(1);
  }
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();

  // hide mouse - useful for RPI!
  #ifdef HIDE_CURSOR
    SDL_ShowCursor(0);
  #endif

  SDL_version v;
  SDL_GetVersion(&v);
  printf("Linked with SDL version: %d.%d.%d\n", v.major, v.minor, v.patch);
  //-------end SDL setup -----------
  font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 16);
  frameDuration = 1000 / TARGET_FPS;
}

Window::~Window() 
{
  for (auto& comp : components) {
    SDL_DestroyTexture(comp.texture);
  }
  Cairo::destroyAllSurfaces();

  SDL_DestroyTexture(screen);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  renderer = NULL;
  window = NULL;
  TTF_CloseFont(font);
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  std::cout << "Booh bye!\n";
}

void Window::setSize(int w, int h) {
  ScreenBase::setSize(w, h);
  SDL_SetWindowSize(window, w, h);
  if (screen != nullptr) {
    SDL_DestroyTexture(screen);
    screen = SDL_CreateTexture(
      renderer,
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STREAMING,
      width,
      height
    );
  }
}

void Window::loop()
{
  screen = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    width,
    height
  );
  Uint32 a = SDL_GetTicks();
  // main
  while (!shouldQuit) {
    handleEvents();
    
    Uint32 b = SDL_GetTicks();
		Uint32 delta = b - a;

    if (delta > frameDuration) {
      updateWindow();
      a = b;
		} else {
      SDL_Delay(frameDuration - delta);
    }
  }

  SDL_DestroyTexture(screen);
  screen = NULL;
}

void Window::handleKeyboardEvent(SDL_Event& evt) {
  if (evt.key.repeat > 0) return;
  auto key = evt.key.keysym.sym;
  switch(key) {
    case SDLK_UP:
      luaInterpreter->triggerPanelCallback(ENCODERS, 1, -1);
      break;
    case SDLK_DOWN:
      luaInterpreter->triggerPanelCallback(ENCODERS, 1, 1);
      break;
    case SDLK_LEFT:
      luaInterpreter->triggerPanelCallback(NAV_BUTTONS, 2, -1);
      break;
    case SDLK_RIGHT:
      luaInterpreter->triggerPanelCallback(NAV_BUTTONS, 3, -1);
      break;
  };
}

void Window::handleEvents() {
  SDL_Event event;
  if (SDL_PollEvent(&event) != 0) {
    LuaRunnerBase::ParamList p;
    switch (event.type) {
      case SDL_QUIT:
        shouldQuit = true;
        break;
      case SDL_KEYDOWN:
        handleKeyboardEvent(event);
      case SDL_MOUSEMOTION:
        luaInterpreter->setMousePos(event.motion.x, event.motion.y);
        break;
      case SDL_MOUSEBUTTONDOWN:
        luaInterpreter->setMouseButton(1);
        break;
      case SDL_MOUSEBUTTONUP:
        luaInterpreter->setMouseButton(0);
        break;
      case SDL_DROPFILE:
        p.push_back({'s', 0, event.drop.file});
        luaInterpreter->callFunction(FILE_DROP, p);
        break;
      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          setSize(event.window.data1, event.window.data2);
          luaInterpreter->setScreenSize(event.window.data1, event.window.data2);
        }
        break;
    }
  }
}

void Window::loadFile(const char* filename) {
  for (auto& comp : components) {
    SDL_DestroyTexture(comp.texture);
  }
  for (auto& comp : liveComponents) {
    SDL_DestroyTexture(comp.texture);
  }
  components.clear();
  liveComponents.clear();
  dirtyTexturesIds.clear();
  luaInterpreter->loadFile(filename);
}

void Window::updateWindow() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  luaInterpreter->updateGlobalVars();
  // check for dirty textures
  for (uint dirtyTexture : dirtyTexturesIds) {
    drawComponent(dirtyTexture);
  }
  dirtyTexturesIds.clear();

  //redraw and render all live components
  for (auto& liveComp : liveComponents) {
    drawLiveComponent(liveComp);
    SDL_RenderCopy(renderer, liveComp.texture, NULL, &liveComp.rect);
  }

  //render normal components
  for (auto& comp : components) {
    SDL_RenderCopy(renderer, comp.texture, NULL, &comp.rect);
  }

  SDL_RenderPresent(renderer);
}

int Window::addComponent(int x, int y, int w, int h, int luaRef) {
  SDL_Texture* new_t = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    w,
    h
  );
  components.push_back({{x, y, w, h}, new_t, luaRef});
  return components.size() - 1;
}

void Window::addLiveComponent(int x, int y, int w, int h, int luaRef) {
  SDL_Texture* new_t = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    w,
    h
  );

  void* pixelData;
  int stride;
  SDL_LockTexture(new_t, NULL, &pixelData, &stride);
  uint surfaceId = Cairo::addSurface();

  Cairo::setSurface(surfaceId);
  Cairo::createSurfaceForData(w, h, static_cast<unsigned char*>(pixelData), stride);
  SDL_UnlockTexture(new_t);

  liveComponents.push_back({{x, y, w, h}, new_t, luaRef, surfaceId});
}

void Window::drawComponent(uint textureId) {
  auto& comp = components[textureId];

  //prepare
  void* rawData;
  int stride;
  SDL_LockTexture(comp.texture, NULL, &rawData, &stride);
  Cairo::setDefaultSurface();
  Cairo::createSurfaceForData(comp.rect.w, comp.rect.h, static_cast<unsigned char*>(rawData), stride);

  //draw
  luaInterpreter->callTableRefFunction(comp.luaTableRef, "draw");
  Cairo::flush();
  
  //finish
  SDL_UnlockTexture(comp.texture);
  Cairo::finalize();
}

void Window::drawLiveComponent(Component& component) {
  //prepare
  void* pixelData;
  int stride;
  SDL_LockTexture(component.texture, NULL, &pixelData, &stride);
  Cairo::setSurface(component.surfaceId);
  Cairo::clearSurface();

  //draw
  luaInterpreter->callTableRefFunction(component.luaTableRef, "draw");
  Cairo::flush();
  
  //copy buffers and finish
  auto refreshedSurface = Cairo::getSurfaceData();
  memcpy(refreshedSurface, pixelData, stride * component.rect.h);
  SDL_UnlockTexture(component.texture);
}

/**
 * Lua Logic
 */

// creates a "live" components - it's redrawn every frame, making it simpler
int Window::_addLiveComponent(lua_State* l) {
  lua_getglobal(l, "Window");
  auto window = reinterpret_cast<Window*>(lua_touserdata(l, -1));
  lua_pop(l, 1);
  luaL_checktype(l, 1, LUA_TTABLE);

  int x = LuaRunnerBase::getTableIntValue(l, 1, "x");
  int y = LuaRunnerBase::getTableIntValue(l, 1, "y");
  int w = LuaRunnerBase::getTableIntValue(l, 1, "w");
  int h = LuaRunnerBase::getTableIntValue(l, 1, "h");

  lua_pushvalue(l, 1);
  int ref = luaL_ref(l, LUA_REGISTRYINDEX);
  window->addLiveComponent(x, y, w, h, ref);

  return 1;
}

// creates a component and return it's ID
int Window::_addComponent(lua_State* l) {
  // retrieve Window* from the registered global variabled
  lua_getglobal(l, "Window");
  auto window = reinterpret_cast<Window*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  luaL_checktype(l, 1, LUA_TTABLE);

  int x = LuaRunnerBase::getTableIntValue(l, 1, "x");
  int y = LuaRunnerBase::getTableIntValue(l, 1, "y");
  int w = LuaRunnerBase::getTableIntValue(l, 1, "w");
  int h = LuaRunnerBase::getTableIntValue(l, 1, "h");

  // create a reference for the component table and keep it in the Component struct
  lua_pushvalue(l, 1);
  int tableRef = luaL_ref(l, LUA_REGISTRYINDEX);
  int textureId = window->addComponent(x, y, w, h, tableRef);

  // add the textureId as a property of the incoming lua table
  lua_pushstring(l, "texture_id");
  lua_pushnumber(l, textureId);
  lua_settable(l, 1);

  // add a "set" method to hide "mark_dirty" from impl.
  lua_pushstring(l, "set");
  lua_pushcfunction(l, [](lua_State* L) -> int
  {
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TSTRING);
    luaL_checkany(L, 3);
    // update table property
    lua_settable(L, 1);
    // retrieve texture_id and alwaysRedraw
    int textureId = LuaRunnerBase::getTableIntValue(L, 1, "texture_id");
    lua_settop(L, 0);

    //now mark texture as dirty!
    lua_getglobal(L, "Window");
    auto w = reinterpret_cast<Window*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    w->addDirtyTexture(textureId);

    return 0;
  });
  lua_settable(l, 1);
  window->drawComponent(textureId);

  return 1;
}
