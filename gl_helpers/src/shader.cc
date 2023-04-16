#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog-inl.h>

GLuint make_shader(const GLchar *vertex_shader_source[],
                   const GLchar *geometry_shader_source[],
                   const GLchar *fragment_shader_source[],
                   std::string &error_msg);

uint32_t compile_shader(GLenum type, const GLchar *const *source, std::string &compile_errors);


// With geometry
Shader::Shader(const GLchar *vertex_shader_source[],
               const GLchar *fragment_shader_source[],
               const GLchar *geometry_shader_source[]
) {
  is_ready_ = false;
  id_ = make_shader(vertex_shader_source, geometry_shader_source, fragment_shader_source, error_msgs_);
  if (!id_) {
    spdlog::error(error_msgs_);
  }
  is_ready_ = true;
}

Shader::~Shader() {
  glDeleteShader(id_);
}

// use/activate the shader
void Shader::use() const {
  if (is_ready_) {
    glUseProgram(id_);
    return;
  }
  spdlog::error("Shader is not ready to run");
}

inline GLint get_uniform_loc_with_logging(const char *type, GLuint prog_id, const char *name) {
  auto loc = glGetUniformLocation(prog_id, name);
  if (loc == -1) spdlog::error("Couldn't find uniform {}:{}", name, type);
  return loc;
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &vec) const {
  auto loc = get_uniform_loc_with_logging("vec3", id_, name.c_str());
  glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(vec));
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) const {
  auto loc = get_uniform_loc_with_logging("matrix4fv", id_, name.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::set_float(const std::string &name, float f) const {
  auto loc = get_uniform_loc_with_logging("1f", id_, name.c_str());
  glUniform1f(loc, f);
}

void Shader::set_int(const std::string &name, int i) const {
  auto loc = get_uniform_loc_with_logging("1i", id_, name.c_str());
  glUniform1i(loc, i);
}

void Shader::set4iv(const std::string &name, int32_t count, const GLint *v4) const {
  auto loc = get_uniform_loc_with_logging("4iv", id_, name.c_str());
  glUniform4iv(loc, count, v4);
}

void Shader::set4b(const std::string &name, bool v0[4]) const {
  auto loc = get_uniform_loc_with_logging("4b", id_, name.c_str());
  glUniform4iv(loc, 1, (const GLint *) v0);
}


void Shader::set2ui(const std::string &name, GLuint v0, GLuint v1) const {
  auto loc = get_uniform_loc_with_logging("2ui", id_, name.c_str());
  glUniform2ui(loc, v0, v1);
}

void Shader::set_float(const std::string &name, float f0, float f1, float f2) const {
  auto loc = get_uniform_loc_with_logging("3f", id_, name.c_str());
  glUniform3f(loc, f0, f1, f2);
}

/**
 * Compile vertex and fragment shaders and link.
 * Include geometry shader where it's supplied.
 *
 * @param vertex_shader_source
 * @param geometry_shader_source
 * @param fragment_shader_source
 * @return
 */
GLuint make_shader(const GLchar *vertex_shader_source[],
                   const GLchar *geometry_shader_source[],
                   const GLchar *fragment_shader_source[],
                   std::string &error_msg) {
  auto shader_program = glCreateProgram();
  if (!shader_program) {
    spdlog::error("Failed to create program [{}]", glGetError());
    return 0;
  }

  std::string compile_error_msg;
  auto vertex_shader = compile_shader(GL_VERTEX_SHADER,
                                      vertex_shader_source,
                                      compile_error_msg);
  if (!vertex_shader) {
    error_msg = fmt::format("Failed to compile vertex shader\n{}", compile_error_msg);
    glDeleteProgram(shader_program);
    return 0;
  }

  auto fragment_shader = compile_shader(GL_FRAGMENT_SHADER,
                                        fragment_shader_source,
                                        compile_error_msg);
  if (!fragment_shader) {
    error_msg = fmt::format("Failed to compile fragment shader\n{}", compile_error_msg);
    glDeleteShader(vertex_shader);
    glDeleteProgram(shader_program);
    return 0;
  }

  if (geometry_shader_source) {
    auto geometry_shader = compile_shader(GL_GEOMETRY_SHADER, geometry_shader_source, compile_error_msg);
    if (!geometry_shader) {
      error_msg = fmt::format("Failed to compile geometry shader\n{}", compile_error_msg);
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader);
      glDeleteProgram(shader_program);
      return 0;
    } else {
      glAttachShader(shader_program, geometry_shader);
      glDeleteShader(geometry_shader);
    }
  }

  glAttachShader(shader_program, vertex_shader);
  glDeleteShader(vertex_shader);

  glAttachShader(shader_program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(shader_program);
  int32_t success;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

  if (success) {
    error_msg = "";
    return shader_program;
  }

  int32_t log_length;
  glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);
  char info_log[log_length];
  glGetProgramInfoLog(shader_program, log_length, nullptr, info_log);
  error_msg = fmt::format("Failed to link shader program\n{}", std::string(info_log));
  glDeleteProgram(shader_program);
  return 0;
}

uint32_t compile_shader(GLenum type, const GLchar *const *source, std::string &compile_errors) {
  auto shader = glCreateShader(type);
  if (!shader) {
    auto msg = fmt::format("Couldn't create shader type {} [{}]", type, glGetError());
    spdlog::error(msg);
    return 0;
  }

  glShaderSource(shader, 1, source, nullptr);
  glCompileShader(shader);

  int32_t success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success) {
    compile_errors = "";
    return shader;
  }

  int32_t log_length;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
  char info_log[log_length];
  glGetShaderInfoLog(shader, log_length, nullptr, info_log);
  compile_errors = std::string(info_log);
  glDeleteShader(shader);
  return 0;
}

uint32_t Shader::get_attribute_location(const std::string &attribute_name) {
  if (!id_) return 0;

  return glGetAttribLocation(id_,attribute_name.c_str());
}
