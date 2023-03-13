#include <OpenGL/gl3.h>
#include <spdlog/spdlog-inl.h>

void gl_check_error_and_halt(const std::string &ctx) {
  auto err = glGetError();
  if( err == GL_NO_ERROR) return;

  auto err_msg = fmt::format("{}: Error {}", ctx, err);
  spdlog::critical(err_msg);
  throw std::runtime_error(err_msg);
}

GLuint generate_texture(GLubyte *image_data, GLint texture_width, GLint texture_height) {
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               texture_width, texture_height, 0,
               GL_RGB, GL_UNSIGNED_BYTE,
               image_data);
  gl_check_error_and_halt("generate_texture");
  free(image_data);
  return texture_id;
}