#include "screen-base.h"
#include <iostream>
#include <fcntl.h> //open
#include <unistd.h>  //close (é mole?)
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

class FramebufferDisplay : public ScreenBase {
  std::vector<uint8_t> pixel_data;

  int fb_fd; // framebuffer file descriptor

  void draw(int);
public:
  FramebufferDisplay(int w, int h) : ScreenBase(w, h) {
    fb_fd = open("/dev/fb0", O_RDWR);
    if (fb_fd < 0) {
      std::cerr << "Failed to open framebuffer device." << std::endl;
      exit(1);
    }

  }
  void loop() override;
};