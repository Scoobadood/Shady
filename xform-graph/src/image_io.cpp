#include "image_io.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

#include <string>
#include <OpenGL/gl3.h>
#include <spdlog/spdlog-inl.h>

int32_t
load_image(const std::string &file_name, int &width, int &height, GLubyte *&pixel_data) {
  FILE *f = fopen(file_name.c_str(), "r");
  if( !f ) {
    spdlog::error("File not found: {}", file_name);
    return IO_FAIL;
  }
  int channels;
  pixel_data = stbi_load_from_file(f, &width, &height, &channels, STBI_default);
  fclose(f);
  if (pixel_data == nullptr) {
    spdlog::error("Cannot load image: {}", file_name);
    return IO_FAIL;
  }
  return IO_OK;
}

int32_t
save_png(const std::string &file_name, int width, int height, uint8_t *pixel_data) {
  auto rv = stbi_write_png(file_name.c_str(), width, height,
                           4, pixel_data, width * 4);
  if (rv == 1) return IO_OK;
  return IO_FAIL;
}
