#include "screen-base.h"
#include <iostream>
#include <fcntl.h> //open
#include <unistd.h>  //close (é mole?)
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <chrono>
#include "main.h"

using namespace std::chrono;

class FramebufferDisplay : public ScreenBase {
  std::vector<uint8_t> pixel_data;

  int fb_fd; // framebuffer file descriptor

  milliseconds frameDuration{1000 / TARGET_FPS};

  void draw(int);
public:
  FramebufferDisplay(int w, int h) : ScreenBase(w, h) {
    fb_fd = open("/dev/fb0", O_RDWR);
    if (fb_fd < 0) {
      std::cerr << "Failed to open framebuffer device." << std::endl;
      exit(1);
    }
  }

  void setFps(int fps) override {
    frameDuration = milliseconds(1000 / fps);
  }

  void loop() override;
};