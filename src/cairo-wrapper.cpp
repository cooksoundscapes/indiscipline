#include "cairo-wrapper.h"
#include <string>
#include <chrono>
#include <ctime>
#include <iostream>

cairo_t* Cairo::cr = nullptr;
cairo_surface_t* Cairo::surface = nullptr;

#ifdef USE_SSD1306
  _cairo_format Cairo::defaultFormat = CAIRO_FORMAT_A8;
#else
  _cairo_format Cairo::defaultFormat = CAIRO_FORMAT_ARGB32;
#endif

std::unordered_map<std::string, cairo_surface_t*> Cairo::extraSurfaces;
std::unordered_map<std::string, cairo_operator_t> Cairo::operators = {
  {"clear", CAIRO_OPERATOR_CLEAR},
  {"source", CAIRO_OPERATOR_SOURCE},
  {"over", CAIRO_OPERATOR_OVER},
  {"in", CAIRO_OPERATOR_IN},
  {"out", CAIRO_OPERATOR_OUT},
  {"atop", CAIRO_OPERATOR_ATOP},
  {"dest", CAIRO_OPERATOR_DEST},
  {"dest_over", CAIRO_OPERATOR_DEST_OVER},
  {"dest_in", CAIRO_OPERATOR_DEST_IN},
  {"dest_out", CAIRO_OPERATOR_DEST_OUT},
  {"dest_atop", CAIRO_OPERATOR_DEST_ATOP},
  {"xor", CAIRO_OPERATOR_XOR},
  {"add", CAIRO_OPERATOR_ADD},
  {"saturate", CAIRO_OPERATOR_SATURATE},
  {"multiply", CAIRO_OPERATOR_MULTIPLY},
  {"screen", CAIRO_OPERATOR_SCREEN},
  {"overlay", CAIRO_OPERATOR_OVERLAY},
  {"darken", CAIRO_OPERATOR_DARKEN},
  {"lighten", CAIRO_OPERATOR_LIGHTEN},
  {"color_dodge", CAIRO_OPERATOR_COLOR_DODGE},
  {"color_burn", CAIRO_OPERATOR_COLOR_BURN},
  {"hard_light", CAIRO_OPERATOR_HARD_LIGHT},
  {"soft_light", CAIRO_OPERATOR_SOFT_LIGHT},
  {"difference", CAIRO_OPERATOR_DIFFERENCE},
  {"exclusion", CAIRO_OPERATOR_EXCLUSION},
  {"hsl_hue", CAIRO_OPERATOR_HSL_HUE},
  {"hsl_saturation", CAIRO_OPERATOR_HSL_SATURATION},
  {"hsl_color", CAIRO_OPERATOR_HSL_COLOR},
  {"hsl_luminosity", CAIRO_OPERATOR_HSL_LUMINOSITY}
};
std::unordered_map<std::string, cairo_line_cap_t> Cairo::lineCaps = {
  {"butt", CAIRO_LINE_CAP_BUTT},
  {"round", CAIRO_LINE_CAP_ROUND},
  {"square", CAIRO_LINE_CAP_SQUARE}
};
std::unordered_map<std::string, PangoAlignment> Cairo::textAlignments = {
  {"left", PANGO_ALIGN_LEFT},
  {"right", PANGO_ALIGN_RIGHT},
  {"center", PANGO_ALIGN_CENTER}
};

int Cairo::getStrideForWidth(int width) {
  return cairo_format_stride_for_width(defaultFormat, width);
}

void Cairo::createSurfaceForData(int w, int h, unsigned char* pixels, int stride)
{
  surface = cairo_image_surface_create_for_data(
    pixels,
    defaultFormat,
    w, h, stride
  );
  cr = cairo_create(surface);
  cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint(cr);
  cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
}

void Cairo::print() {
  std::string path{getenv("HOME")};

  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  auto tstamp = std::ctime(&time);
  
  path += "/indiscipline_" + std::string(tstamp) + ".png";

  auto s = cairo_surface_write_to_png(surface, path.c_str()); 
  if (s != CAIRO_STATUS_SUCCESS) {
    std::cerr << "Error while saving to file: " << cairo_status_to_string(s) << '\n';
  }
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
void Cairo::new_path() {
  cairo_new_path(cr);
}
void Cairo::close_path() {
  cairo_close_path(cr);
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
void Cairo::fill_preserve() {
  cairo_fill_preserve(cr);
}
void Cairo::stroke() {
  cairo_stroke(cr);
}
void Cairo::set_operator(std::string op) {
  if (operators.find(op) == operators.end()) return;
  cairo_set_operator(cr, operators[op]);
}
void Cairo::set_line_cap(std::string type) {
  if (lineCaps.find(type) == lineCaps.end()) return;
  cairo_set_line_cap(cr, lineCaps[type]);
}

void Cairo::text(TextParams& params)
{
  // should optimize view in OLED display, verify if it's needed in fbdev
  if (!params.enableAntiAlias) {
    cairo_font_options_t *options = cairo_font_options_create();
    cairo_font_options_set_antialias(options, CAIRO_ANTIALIAS_NONE);
    //cairo_font_options_set_hint_style(options, CAIRO_HINT_STYLE_FULL);

    cairo_set_font_options(cr, options);
    cairo_font_options_destroy(options);
  }
  //-----------------------
  PangoLayout* layout = pango_cairo_create_layout(cr);
  pango_layout_set_text(layout, params.text, -1);
  if (textAlignments.find(params.alignment) != textAlignments.end()) {
    pango_layout_set_alignment(layout, textAlignments[params.alignment]);
  }

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

void Cairo::create_additional_surface(std::string name, double w, double h) {
  auto it = extraSurfaces.find(name);
  if (it != extraSurfaces.end()) {
    destroy_surface(name);
  }

  extraSurfaces.insert({
    name,
    cairo_image_surface_create(defaultFormat, w, h)
  });
  cr = cairo_create(extraSurfaces[name]);
  cairo_close_path(cr);
}

void Cairo::draw_surface(std::string name, double x, double y)
{
  auto it = extraSurfaces.find(name);
  if (it == extraSurfaces.end()) return;

  cairo_set_source_surface(cr, extraSurfaces[name], x, y);
  cairo_paint(cr);
}

void Cairo::destroy_surface(std::string name) {
  auto it = extraSurfaces.find(name);
  if (it == extraSurfaces.end()) return;

  cairo_surface_destroy(extraSurfaces[name]);
  extraSurfaces.erase(name);
}
