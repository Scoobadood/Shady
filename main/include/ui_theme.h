#ifndef IMAGE_TOYS_UI_THEME_H
#define IMAGE_TOYS_UI_THEME_H

#include "imgui.h"

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

#endif //IMAGE_TOYS_UI_THEME_H
