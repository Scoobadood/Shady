#ifndef GLHELPERS_SHADER_H
#define GLHELPERS_SHADER_H

#include <string>
#include "OpenGL/gl3.h"
#include "glm/glm.hpp"

class Shader {
public:
  // With geometry and source
  Shader(const char *vertex_shader_source[],
         const char *fragment_shader_source[],
         const char *geometry_shader_source[] = nullptr);

  ~Shader();

  // @return true if the shader is ready.
  inline bool is_good() const { return is_ready_;}

  // use/activate the shader
  void use() const;

  // get_attribute_location
  uint32_t get_attribute_location(const std::string& attribute_name);

  // utility uniform functions
  void set_int(const std::string &name, int32_t value) const;

  void set_float(const std::string &name, float value) const;

  void set_float(const std::string &name, float f0, float f1, float f2) const;

  void set4iv(const std::string &name, int32_t count, const GLint *v4) const;

  void set4b(const std::string &name, bool v0[4]) const;

  void set2ui(const std::string &name, GLuint v0, GLuint v1) const;

  void set_vec3(const std::string &name, const glm::vec3 &vec) const;

  void set_mat4(const std::string &name, const glm::mat4 &mat) const;

  inline const std::string &get_error() const { return error_msgs_; }

private:
  // The program ID
  uint32_t id_;

  // Flag indicating that the shader is ready to run
  bool is_ready_;

  // Log of any compile or link errors
  std::string error_msgs_;
};

#endif //GLHELPERS_SHADER_H
