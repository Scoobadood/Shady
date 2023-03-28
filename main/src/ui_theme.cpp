#include "ui_theme.h"
#include "image_io.h"

float g_rounding = 5.0f;
ImVec4 g_xform_bg_colour = ImColor(40, 40, 40, 255);
ImVec4 g_title_bg_colour = ImColor(57, 57, 57, 255);
ImVec4 g_text_colour = ImColor(140, 140, 140, 255);
ImVec4 g_in_port_bg_colour = ImColor(57, 57, 57, 255);
ImVec4 g_out_port_bg_colour = ImColor(43, 43, 43, 255);
ImVec4 g_unconfigured_xf_border = ImColor(0, 80, 80, 255);
ImVec4 g_invalid_xf_border = ImColor(250, 0, 0, 255);
ImVec4 g_stale_xf_border = ImColor(250, 240, 0, 255);
ImVec4 g_good_xf_border = ImColor(0, 80, 0, 255);

ImColor g_conn_in_port_colour = ImColor(170, 230, 150, 255);
ImColor g_conn_out_port_colour = ImColor(230, 170, 150, 255);


void init_global_style() {
  auto &style = ImGui::GetStyle();
  style.WindowRounding = g_rounding;
  style.Colors[ImGuiCol_TitleBg] = g_title_bg_colour;
  style.Colors[ImGuiCol_TitleBgActive] = g_title_bg_colour;
  style.Colors[ImGuiCol_Text] = g_text_colour;
  style.Colors[ImGuiCol_WindowBg] = g_xform_bg_colour;
  style.Colors[ImGuiCol_ChildBg] = g_xform_bg_colour;
  style.Colors[ImGuiCol_Border] = g_xform_bg_colour;
  style.Colors[ImGuiCol_PopupBg] = g_xform_bg_colour;
  style.ChildBorderSize = 0.0f;
  style.WindowBorderSize = 2.0f;
}

/*
 * Load icons
 */
GLuint load_icons(const std::string & icon_file) {
  int32_t width, height;
  uint8_t *image_bytes;
  if (IO_OK != load_image(icon_file, width, height, image_bytes)) {
    return 0;
  }

  GLuint tx;
  // load file to texture
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &tx);
  glBindTexture(GL_TEXTURE_2D, tx);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               width, height, 0,
               GL_RGB, GL_UNSIGNED_BYTE,
               image_bytes);
  free(image_bytes);
  return tx;
}

Theme::Theme(const std::string & theme) {
  btn_tx_ = load_icons(theme);
}

Theme::~Theme() {
  glDeleteTextures(1, &btn_tx_);
}

std::shared_ptr<Theme> Theme::theme() {
  return std::make_shared<Theme>("/Users/dave/CLionProjects/image_toys/data/icons.png");
}
