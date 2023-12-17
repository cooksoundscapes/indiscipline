#include "screen-base.h"

class FramebufferDisplay : public ScreenBase {
  int targetFps = 60;

  std::vector<uint8_t> pixel_data;

  void draw(int);
public:
  FramebufferDisplay(int w, int h) : ScreenBase(w, h) {}
  void loop() override;
};