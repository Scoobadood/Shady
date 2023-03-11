//
// Created by Dave Durbin on 11/3/2023.
//

#ifndef IMAGE_TOYS_GLFW_SUPPORT_H
#define IMAGE_TOYS_GLFW_SUPPORT_H

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <string>

void initGl();

GLFWwindow *create_window(const std::string &title, int32_t width, int32_t height);

void init_buffers(GLuint &vao_id, GLuint &vbo_verts, GLuint &vbo_indices);

GLuint generate_texture(GLubyte *image_data, GLint texture_width, GLint texture_height);

void tidy_up(GLuint vbo_verts, GLuint vbo_indices, GLuint vao_id, GLuint texture_id, GLFWwindow * window);

#endif //IMAGE_TOYS_GLFW_SUPPORT_H
