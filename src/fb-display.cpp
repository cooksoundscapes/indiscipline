#include "fb-display.h"
#include "cairo-wrapper.h"
#include <thread>

void FramebufferDisplay::loop() {
  // prepare pixel data allocation
  int stride = Cairo::getStrideForWidth(width);
  pixel_data.resize(stride * height);

  auto b = high_resolution_clock::now(); 
  
  while (!shouldQuit) {	
    auto a = high_resolution_clock::now();
    auto delta = duration_cast<milliseconds>(a - b);

    if (delta > frameDuration) {
      draw(stride);
    } else {
      std::this_thread::sleep_for(frameDuration - delta);
    }
  }
}

void FramebufferDisplay::draw(int stride) {
  pixel_data.assign(pixel_data.size(), 0);
  Cairo::createSurfaceForData(
    width, height,
    pixel_data.data(),
    stride
  );
  if (luaInterpreter != nullptr) {
    luaInterpreter->draw();
  }
  Cairo::flush();
  Cairo::finalize();

  // Reset the file pointer to the beginning of the framebuffer device
  lseek(fb_fd, 0, SEEK_SET);

  if (write(fb_fd, pixel_data.data(), pixel_data.size()) < 0) {
    perror("[fb write]");
    exit(1);
  }
}