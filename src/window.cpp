#include "lua-runner-base.h"
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

  SDL_version v;
  SDL_GetVersion(&v);
  printf("Linked with SDL version: %d.%d.%d\n", v.major, v.minor, v.patch);
  //-------end SDL setup -----------
  font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 16);
  frameDuration = 1000 / TARGET_FPS;
}

Window::~Window() 
{
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

void Window::updateWindow() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  draw();
  SDL_RenderCopy(renderer, screen, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void Window::draw() {
  //prepare texture data
  void* rawData;
  int stride;
  SDL_LockTexture(screen, NULL, &rawData, &stride);
  auto pixels = static_cast<unsigned char*>(rawData);

  Cairo::createSurfaceForData(width, height, pixels, stride);

  if (luaInterpreter != nullptr) {
    luaInterpreter->draw();
  }

  //end drawing
  SDL_UnlockTexture(screen);
  Cairo::flush();
  Cairo::finalize();
}
