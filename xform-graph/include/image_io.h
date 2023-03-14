#ifndef IMAGE_TOYS_IMAGE_IO_H
#define IMAGE_TOYS_IMAGE_IO_H

#include <OpenGL/gl3.h>
#include <string>

GLubyte *
load_image(const std::string &file_name, int &width, int &height );


int save_png(const std::string &file_name, int width, int height, uint8_t * pixels_data );

#endif //IMAGE_TOYS_IMAGE_IO_H
