#include "window.h"
#include "cairo-wrapper.h"
#include <iostream>
#include <SDL2/SDL_image.h>

Window::Window(int w, int h) : w{w}, h{h}
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    std::cerr << SDL_GetError() << '\n';
  window = SDL_CreateWindow(
    "HW::CTRL", 
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    w,
    h,
    SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE
  );
  if (window == NULL)
    std::cerr << SDL_GetError() << '\n';
      
  renderer = SDL_CreateRenderer(window, -1, 
    SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED); 
  if (renderer == NULL)
    std::cerr << SDL_GetError() << '\n';  
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();
  //-------end SDL setup -----------
  font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 16);

  SDL_version linked;
  SDL_GetVersion(&linked);
  printf("Linked with SDL version: %d.%d.%d\n", linked.major, linked.minor, linked.patch);

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

void Window::loop()
{
  screen = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    w,
    h
  );
  auto start = SDL_GetTicks();
  auto end = SDL_GetTicks();
  float delta = 0;
    
  while (!shouldQuit) {
    start = SDL_GetTicks();
		delta = start - end;
    if (delta > 1000/30.0) {
			handleEvents();
			updateWindow();
			end = start;
			fps = 1000 / delta;
			luaInterpreter->setGlobal("fps", fps);
		}
  }
  SDL_DestroyTexture(screen);
  screen = NULL;
}

void Window::handleEvents() {
  if (SDL_PollEvent(&event_handler) != 0) {
    switch (event_handler.type) {
      case SDL_QUIT:
        shouldQuit = true;
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

  Cairo::createSurfaceForData_ARGB32(w, h, pixels, stride);

  if (luaInterpreter != nullptr)
    luaInterpreter->draw();

  //end drawing
  SDL_UnlockTexture(screen);
  Cairo::finalize();
}

void Window::loadLuaScript(std::string file) {
  luaInterpreter->loadFile(file);
}
