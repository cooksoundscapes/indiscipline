#include "cairo-wrapper.h"
#include <iostream>
#include <string>
#include <pango/pangocairo.h>

cairo_t* Cairo::cr = nullptr;
cairo_surface_t* Cairo::surface = nullptr;
_cairo_format Cairo::defaultFormat = CAIRO_FORMAT_A8;
std::unordered_map<std::string, cairo_surface_t*> Cairo::extraSurfaces;

int Cairo::getStrideForWidth_A1(int width) {
  return cairo_format_stride_for_width(CAIRO_FORMAT_A1, width);
}
int Cairo::getStrideForWidth_A8(int width) {
  return cairo_format_stride_for_width(CAIRO_FORMAT_A8, width);
}

void Cairo::createSurfaceForData(int w, int h, unsigned char* pixels, int stride, cairo_format_t format)
{
  surface = cairo_image_surface_create_for_data(
    pixels,
    format,
    w, h, stride
  );
  cr = cairo_create(surface);
}
void Cairo::createSurfaceForData_ARGB32(int w, int h, unsigned char* pixels, int stride) {
  createSurfaceForData(w, h, pixels, stride, CAIRO_FORMAT_ARGB32);
}
void Cairo::createSurfaceForData_A1(int w, int h, unsigned char* pixels, int stride) {
  createSurfaceForData(w, h, pixels, stride, CAIRO_FORMAT_A1);
}
void Cairo::createSurfaceForData_A8(int w, int h, unsigned char* pixels, int stride) {
  createSurfaceForData(w, h, pixels, stride, CAIRO_FORMAT_A8);
}

void Cairo::finalize() {
  if (cr)
    cairo_destroy(cr);
  if (surface)
    cairo_surface_destroy(surface);
}

void Cairo::flush() {
  cairo_surface_flush(surface);
}

void Cairo::clearExtraSurfaces() {
  for (auto entry : extraSurfaces) {
    if (entry.second) 
      cairo_surface_destroy(entry.second);
    extraSurfaces.erase(entry.first);
  }
}

void Cairo::set_source_rgb(double r, double g, double b) {
  cairo_set_source_rgb(cr, r, g, b);
} 
void Cairo::set_source_rgba(double r, double g, double b, double a) {
  cairo_set_source_rgba(cr, r, g, b, a);
}
void Cairo::rectangle(double x, double y, double w, double h) {
  cairo_rectangle(cr, x, y, w, h);
}
void Cairo::arc(double xc, double yc, double radius, double angle1, double angle2) {
  cairo_arc(cr, xc, yc, radius, angle1, angle2);
}
void Cairo::move_to(double x, double y) {
  cairo_move_to(cr, x, y);
}
void  Cairo::line_to(double x, double y) {
  cairo_line_to(cr, x, y);
}
void Cairo::rel_move_to(double x, double y) {
  cairo_rel_move_to(cr, x, y);
}
void  Cairo::rel_line_to(double x, double y) {
  cairo_rel_line_to(cr, x, y);
}
void Cairo::set_line_width(double w) {
  cairo_set_line_width(cr, w);
}
void Cairo::paint() {
  cairo_paint(cr);
}
void Cairo::fill() {
  cairo_fill(cr);
}
void Cairo::stroke() {
  cairo_stroke(cr);
}

void Cairo::text(TextParams& params)
{
  // should optimize view in OLED display, verify if it's needed in fbdev
  cairo_font_options_t *options = cairo_font_options_create();
  cairo_font_options_set_antialias(options, CAIRO_ANTIALIAS_NONE);
  cairo_set_font_options(cr, options);
  cairo_font_options_destroy(options);
  //-----------------------

  PangoLayout* layout = pango_cairo_create_layout(cr);
  pango_layout_set_text(layout, params.text, -1);

  std::string font = std::string(params.font) + " " + std::to_string(params.size);
  PangoFontDescription* desc = pango_font_description_from_string(font.c_str());
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  int off_x{0}, txt_size, txt_height;
  pango_layout_get_size(layout, &txt_size, &txt_height);

  if (params.centered) {
    off_x = params.width/2 - txt_size/PANGO_SCALE/2;
    cairo_rel_move_to(cr, off_x, 0);
  }
  if (params.width > 0) {
    pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_width(layout, (int)(params.width*PANGO_SCALE));  
  }
  pango_cairo_show_layout(cr, layout);

  g_object_unref(layout);
}

void Cairo::createAdditionalSurface(std::string name, double w, double h) {
  auto it = extraSurfaces.find(name);
  if (it != extraSurfaces.end()) {
    destroySurface(name);
  }

  extraSurfaces.insert({
    name,
    cairo_image_surface_create(defaultFormat, w, h)
  });
  cr = cairo_create(extraSurfaces[name]);
  cairo_close_path(cr);
}

void Cairo::drawSurface(std::string name, double x, double y)
{
  auto it = extraSurfaces.find(name);
  if (it == extraSurfaces.end()) return;

  cairo_set_source_surface(cr, extraSurfaces[name], x, y);
  cairo_paint(cr);
}

void Cairo::destroySurface(std::string name) {
  auto it = extraSurfaces.find(name);
  if (it == extraSurfaces.end()) return;

  cairo_surface_destroy(extraSurfaces[name]);
  extraSurfaces.erase(name);
}
