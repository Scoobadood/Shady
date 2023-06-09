/*
 * This is an abstract base class for Xforms that accept a single input
 * and produce a single output and run a single shader program to transform
 * between them.
 *
 * Subclasses should specify the XformConfig required to configure the shader as
 * well as supplying source code for the shaders themselves.
 */
#ifndef SHADY_SINGLE_IO_SHADER_XFORM_H
#define SHADY_SINGLE_IO_SHADER_XFORM_H

#include "render-xform.h"
#include "shader.h"

class SingleIOShaderXform : public RenderXform {
public:
  explicit SingleIOShaderXform(const std::string &name, XformConfig config);

  ~SingleIOShaderXform() override;

  /**
   * Virtual initialisation; MUST be called before object can be used.
   */
  void init() override;

protected:
  void configure_framebuffer() override;
  std::unique_ptr<Shader> shader_;

private:
  virtual void init_shader() = 0;

  virtual void bind_shader_variables() = 0;

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs, uint32_t &err, std::string &err_msg) override;

  GLuint texture_id_;
};

#endif //SHADY_SINGLE_IO_SHADER_XFORM_H
