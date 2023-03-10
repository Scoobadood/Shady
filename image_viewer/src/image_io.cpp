#include "image_io.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <string>
#include <OpenGL/gl3.h>
#include <spdlog/spdlog-inl.h>

GLubyte *
load_image(const std::string &file_name, int &width, int &height) {
  FILE *f = fopen(file_name.c_str(), "r");
  int channels;
  GLubyte *pData = stbi_load_from_file(f, &width, &height, &channels, STBI_default);
  if (pData == nullptr) {
    spdlog::error("Cannot load image: {}", file_name);
    exit(EXIT_FAILURE);
  }


  /* Flip vertically */
  for (int src_row = 0; src_row * 2 < height; ++src_row) {
    int src_idx = src_row * width * channels;
    int dst_idx = (height - 1 - src_row) * width * channels;
    for (int byte_idx = width * channels; byte_idx > 0; --byte_idx) {
      GLubyte temp = pData[src_idx];
      pData[src_idx] = pData[dst_idx];
      pData[dst_idx] = temp;
      ++src_idx;
      ++dst_idx;
    }
  }
  return pData;
}