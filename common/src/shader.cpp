#include "shader.h"
#include "glm/gtc/type_ptr.hpp"

#include <glm/glm.hpp>

#include <spdlog/spdlog-inl.h>

uint32_t compile_shader(GLenum type, const GLchar *const *source) {
  auto shader = glCreateShader(type);
  if (!shader) {
    auto msg = fmt::format("Couldn't create shader type {} [{}]", type, glGetError());
    spdlog::critical(msg);
    throw std::runtime_error(msg);
  }

  glShaderSource(shader, 1, source, nullptr);
  glCompileShader(shader);

  int32_t success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    int32_t log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    char info_log[log_length];
    glGetShaderInfoLog(shader, log_length, nullptr, info_log);
    spdlog::critical("Shader type {} compilation failed", type);
    spdlog::error("{}", (char *) info_log);
  }
  return shader;
}

GLuint Shader::make_shader(const GLchar *vertex_shader_source[],
                           const GLchar *geometry_shader_source[],
                           const GLchar *fragment_shader_source[]) {
  auto vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
  auto fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
  uint32_t shader_program = glCreateProgram();
  if (!shader_program) {
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    spdlog::critical("Failed to create program [{}]", glGetError());
  }

  bool has_geometry_shader = (geometry_shader_source != nullptr);
  if (has_geometry_shader) {
    auto geometry_shader = compile_shader(GL_GEOMETRY_SHADER, geometry_shader_source);
    glAttachShader(shader_program, geometry_shader);
    glDeleteShader(geometry_shader);
  }

  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  int32_t success;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  if (!success) {
    int32_t log_length;
    glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);
    char info_log[log_length];
    glGetProgramInfoLog(shader_program, log_length, nullptr, info_log);

    spdlog::critical("Program linking failed [{}]", glGetError());
    spdlog::error("{}", (char *) info_log);
  }

  is_ready_ = true;
  return shader_program;
}

// With geometry
Shader::Shader(const GLchar *vertex_shader_source[],
               const GLchar *geometry_shader_source[],
               const GLchar *fragment_shader_source[]
) {
  is_ready_ = false;
  ID = make_shader(vertex_shader_source, geometry_shader_source, fragment_shader_source);
}

Shader::~Shader() {
  glDeleteShader(ID);
}

// use/activate the shader
void Shader::use() const {
  if (is_ready_) {
    glUseProgram(ID);
    return;
  }
  spdlog::critical("Shader is not ready to run");
}

inline GLint getUniformLocWithLogging(const char *type, GLuint prog_id, const char *name) {
  auto loc = glGetUniformLocation(prog_id, name);
  if (loc == -1) spdlog::error("Couldn't find uniform {}:{}", name, type);
  return loc;
}

void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const {
  auto loc = getUniformLocWithLogging("vec3", ID, name.c_str());
  glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(vec));
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
  auto loc = getUniformLocWithLogging("matrix4fv", ID, name.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::set1f(const std::string &name, GLfloat v0) const {
  auto loc = getUniformLocWithLogging("1f", ID, name.c_str());
  glUniform1f(loc, v0);
}

void Shader::set1i(const std::string &name, GLint v0) const {
  auto loc = getUniformLocWithLogging("1i", ID, name.c_str());
  glUniform1i(loc, v0);
}

void Shader::set4iv(const std::string &name, int32_t count, const GLint *v4) const {
  auto loc = getUniformLocWithLogging("4iv", ID, name.c_str());
  glUniform4iv(loc, count, v4);
}

void Shader::set4b(const std::string &name, bool v0[4]) const {
  auto loc = getUniformLocWithLogging("4b", ID, name.c_str());
  glUniform4iv(loc, 1, (const GLint*)v0);
}


void Shader::set2ui(const std::string &name, GLuint v0, GLuint v1) const {
  auto loc = getUniformLocWithLogging("2ui", ID, name.c_str());
  glUniform2ui(loc, v0, v1);
}

void Shader::set3f(const std::string &name, GLfloat v0, GLfloat v1, GLfloat v2) const {
  auto loc = getUniformLocWithLogging("3f", ID, name.c_str());
  glUniform3f(loc, v0, v1, v2);
}


