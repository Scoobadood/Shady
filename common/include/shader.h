#ifndef IMAGE_TOY_COMMON_SHADER_H_
#define IMAGE_TOY_COMMON_SHADER_H_

#include <string>
#include <glm/glm.hpp>
#include <OpenGL/gl3.h>

class Shader {
 public:
  // the program ID
  unsigned int ID;

  // With geometry and source
  Shader(const GLchar *vertex_shader_source[],
         const GLchar *geometry_shader_source[],
         const GLchar *fragment_shader_source[]
  );

  ~Shader();

  // use/activate the shader
  void use() const;

  // utility uniform functions
  void set1i(const std::string &name, GLint v0) const;
  void set1f(const std::string &name, GLfloat v0) const;
  void set2ui(const std::string &name, GLuint v0, GLuint v1) const;
  void set3f(const std::string &name, GLfloat v0, GLfloat v1, GLfloat v2) const;
  void setVec3(const std::string &name, const glm::vec3 &vec) const;
  void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
  GLuint make_shader(const GLchar *vertex_shader_source[],
              const GLchar *geometry_shader_source[],
              const GLchar *fragment_shader_source[]);
  bool is_ready_;
};

#endif // IMAGE_TOY_COMMON_SHADER_H_
