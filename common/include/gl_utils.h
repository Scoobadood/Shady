#ifndef IMAGE_TOYS_COMMON_GL_UTILS_H
#define IMAGE_TOYS_COMMON_GL_UTILS_H

#include <string>
void gl_check_error_and_halt(const std::string &ctx);

GLuint generate_texture(GLubyte *image_data, GLint texture_width, GLint texture_height);

#endif //IMAGE_TOYS_COMMON_GL_UTILS_H
