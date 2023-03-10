//
// Created by Dave Durbin on 15/12/2022.
//

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

void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const {
  glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(vec));
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::set1i(const std::string &name, GLint v0) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), v0);
}

void Shader::set2ui(const std::string &name, GLuint v0, GLuint v1) const {
  glUniform2ui(glGetUniformLocation(ID, name.c_str()), v0, v1);
}

void Shader::set3f(const std::string &name, GLfloat v0, GLfloat v1, GLfloat v2) const {
  glUniform3f(glGetUniformLocation(ID, name.c_str()), v0, v1, v2);
}


