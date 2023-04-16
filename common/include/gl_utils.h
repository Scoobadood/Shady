#ifndef SHADY_COMMON_GL_UTILS_H
#define SHADY_COMMON_GL_UTILS_H

#include <string>
#include <OpenGL/gl3.h>

void gl_check_error_and_halt(const std::string &ctx);

GLuint generate_texture(GLubyte *image_data, GLint texture_width, GLint texture_height);

#endif //SHADY_COMMON_GL_UTILS_H
