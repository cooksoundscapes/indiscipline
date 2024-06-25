#include "spi-display.h"
#include <thread>
#include "cairo-wrapper.h"

void SPIDisplay::loop() {
  // prepare pixel data allocation
  int stride = Cairo::getStrideForWidth(width);
  pixel_data.resize(stride * height);
    
  while (!shouldQuit) {
    auto start = high_resolution_clock::now();
		
    draw(stride);
    if (device)
      device->update(pixel_data);

    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<milliseconds>(end - start);
    
    if (elapsed < frameDuration) {
      std::this_thread::sleep_for(frameDuration - elapsed);
    }
  }
}

void SPIDisplay::draw(int stride) {
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