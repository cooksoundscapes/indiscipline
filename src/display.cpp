#include "display.h"
#include <chrono>
#include <thread>
#include "cairo-wrapper.h"

using namespace std::chrono;

void Display::loop() {
  // setup FPS
  int targetFps = 60;
  milliseconds frameDuration(1000 / targetFps);

  // prepare pixel data allocation
  int stride = Cairo::getStrideForWidth_A8(width);
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

/**
 * @deprecated (almost)
*/
void Display::measureFps() {
  static int frameCount = 0;
  static auto lastSecond = high_resolution_clock::now();

  frameCount++;
  auto now = high_resolution_clock::now();
  auto elapsedSeconds = duration_cast<seconds>(now - lastSecond);
  if (elapsedSeconds.count() >= 1) {
    double actualFPS = frameCount / elapsedSeconds.count();
    frameCount = 0;
    lastSecond = now;
    luaInterpreter->setGlobal("fps", actualFPS);
  }
}

void Display::draw(int stride) {
  pixel_data.assign(pixel_data.size(), 0);
  Cairo::createSurfaceForData_A8(
    width, height,
    pixel_data.data(),
    stride
  );
  if (luaInterpreter != nullptr)
    luaInterpreter->draw();
  Cairo::flush();
  Cairo::finalize();
}

void Display::loadLuaScript(std::string file) {
  luaInterpreter->loadFile(file);
}
