#include "fb-display.h"
#include "cairo-wrapper.h"
#include <thread>
#include <iostream>

void FramebufferDisplay::loop() {
  // prepare pixel data allocation
  int stride = Cairo::getStrideForWidth(width);
  pixel_data.resize(stride * height);

  auto a = high_resolution_clock::now(); 
  
  while (!shouldQuit) {	
    auto b = high_resolution_clock::now();
    auto delta = duration_cast<milliseconds>(b - a);

    if (delta > frameDuration) {
      draw(stride);
      a = b;
    } else {
      std::this_thread::sleep_for(frameDuration - delta);
    }
  }
}

void FramebufferDisplay::draw(int stride) {
  pixel_data.assign(pixel_data.size(), 0);
  Cairo::createSurfaceForData(
    0, width, height,
    pixel_data.data(),
    stride
  );
  Cairo::setDefaultSurface();
  luaInterpreter->callFunction(DRAW);
  Cairo::flush();
  Cairo::finalize();

  // Reset the file pointer to the beginning of the framebuffer device
  lseek(fb_fd, 0, SEEK_SET);

  if (write(fb_fd, pixel_data.data(), pixel_data.size()) < 0) {
    perror("[fb write]");
    exit(1);
  }
}

void FramebufferDisplay::loadFile(const char* name) {
  luaInterpreter->loadFile(name);
}
