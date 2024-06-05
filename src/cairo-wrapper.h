#pragma once
#include <cairo/cairo.h>
#include <pango/pangocairo.h>
#include <unordered_map>
#include <string>

namespace Cairo {
  extern cairo_t* cr;
  extern cairo_surface_t* surface;
  extern std::unordered_map<std::string, cairo_surface_t*> extraSurfaces;
  extern _cairo_format defaultFormat;
  extern std::unordered_map<std::string, cairo_operator_t> operators;
  extern std::unordered_map<std::string, cairo_line_cap_t> lineCaps;
  extern std::unordered_map<std::string, PangoAlignment> textAlignments;

  extern int getStrideForWidth(int w);
  extern void createSurfaceForData(int w, int h, unsigned char* pixels, int stride);
  extern void finalize();
  extern void flush();
  extern void clearExtraSurfaces();

  extern void set_source_rgb(double, double, double);
  extern void set_source_rgba(double, double, double, double);
  extern void new_path();
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
  extern void set_operator(std::string);
  extern void set_line_cap(std::string);

  extern void create_additional_surface(std::string, double, double);
  extern void draw_surface(std::string, double, double);
  extern void destroy_surface(std::string);

  struct TextParams {
    const char* text;
    const char* font;
    const char * alignment;
    int size;
    int width;
    bool centered;
    bool enableAntiAlias;
  };
  extern void text(TextParams&);
}