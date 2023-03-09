#ifndef IMAGE_TOYS_COMMON_GL_UTILS_H
#define IMAGE_TOYS_COMMON_GL_UTILS_H

#include <GLFW/glfw3.h>

GLFWwindow *initGl();

GLFWwindow *create_window(const std::string &title, int32_t width, int32_t height);

#endif //IMAGE_TOYS_COMMON_GL_UTILS_H
