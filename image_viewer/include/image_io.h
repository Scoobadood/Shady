#ifndef IMAGE_TOYS_IMAGE_IO_H
#define IMAGE_TOYS_IMAGE_IO_H

#include <OpenGL/gl3.h>
#include <string>

GLubyte *
load_image(const std::string &file_name, int &width, int &height );

#endif //IMAGE_TOYS_IMAGE_IO_H
