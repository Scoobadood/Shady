#ifndef SHADY_IMAGE_IO_H
#define SHADY_IMAGE_IO_H

#include <OpenGL/gl3.h>
#include <string>

const int32_t IO_OK = 0;
const int32_t IO_FAIL = 1;

int32_t
load_image(const std::string &file_name, int &width, int &height, uint8_t * & pixel_data );


int32_t
save_png(const std::string &file_name, int width, int height, uint8_t * pixel_data );

#endif //SHADY_IMAGE_IO_H
