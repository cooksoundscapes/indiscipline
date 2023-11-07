#pragma once
#include <cairo/cairo.h>

namespace Cairo {
  extern cairo_t* cr;
  extern cairo_surface_t* surface;

  extern int getStrideForWidth_A1(int w);
  extern int getStrideForWidth_A8(int w);
  extern void createSurfaceForData(int w, int h, unsigned char* pixels, int stride, cairo_format_t format);
  extern void createSurfaceForData_ARGB32(int w, int h, unsigned char* pixels, int stride);
  extern void createSurfaceForData_A1(int w, int h, unsigned char* pixels, int stride);
  extern void createSurfaceForData_A8(int w, int h, unsigned char* pixels, int stride);
  extern void finalize();
  extern void flush();

  extern void set_source_rgb(double, double, double);
  extern void set_source_rgba(double, double, double, double);
  extern void rectangle(double, double, double, double);
  extern void arc(double, double, double, double, double);
  extern void move_to(double, double);
  extern void line_to(double, double);
  extern void rel_move_to(double, double);
  extern void rel_line_to(double, double);
  extern void set_line_width(double);
  extern void paint();
  extern void fill();
  extern void stroke();

  extern void text(const char*, double, double, bool);
}