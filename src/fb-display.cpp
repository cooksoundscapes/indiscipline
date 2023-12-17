#include "fb-display.h"
#include "cairo-wrapper.h"
#include <chrono>
#include <thread>

using namespace std::chrono;

void FramebufferDisplay::loop() {
  // setup FPS
  milliseconds frameDuration(1000 / targetFps);

  // prepare pixel data allocation
  int stride = Cairo::getStrideForWidth(width);
  pixel_data.resize(stride * height);
    
  while (!shouldQuit) {
    auto start = high_resolution_clock::now();
		
    draw(stride);

    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<milliseconds>(end - start);
    
    if (elapsed < frameDuration) {
      std::this_thread::sleep_for(frameDuration - elapsed);
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
  if (luaInterpreter != nullptr)
    luaInterpreter->draw();
  Cairo::flush();
  Cairo::finalize();
}