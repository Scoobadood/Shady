#ifndef IMAGE_TOYS_UI_THEME_H
#define IMAGE_TOYS_UI_THEME_H

#include "imgui.h"

#include <OpenGL/gl3.h>
#include <string>

extern float g_rounding;
extern ImVec4 g_xform_bg_colour;
extern ImVec4 g_title_bg_colour;
extern ImVec4 g_text_colour;
extern ImVec4 g_in_port_bg_colour;
extern ImVec4 g_out_port_bg_colour;
extern ImVec4 g_unconfigured_xf_border;
extern ImVec4 g_invalid_xf_border;
extern ImVec4 g_stale_xf_border;
extern ImVec4 g_good_xf_border;

extern ImColor g_conn_in_port_colour;
extern ImColor g_conn_out_port_colour;

struct Theme {
  GLuint btn_tx_;
  explicit Theme(const std::string & theme);
  ~Theme();

  static std::shared_ptr<Theme> theme();
};

void init_global_style();

#endif //IMAGE_TOYS_UI_THEME_H
