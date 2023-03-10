#include <OpenGL/gl3.h>
#include <spdlog/spdlog-inl.h>

void gl_check_error_and_halt(const std::string &ctx) {
  if( glGetError() == GL_NO_ERROR) return;

  auto err = glGetError();
  auto err_msg = fmt::format("{}: Error {}", ctx, err);
  spdlog::critical(err_msg);
  throw std::runtime_error(err_msg);
}