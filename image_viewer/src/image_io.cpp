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
  return pData;
}