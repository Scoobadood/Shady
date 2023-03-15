/*
 * This is an abstract base class for Xforms that accept a single input
 * and produce a single output and run a single shader program to transform
 * between them.
 *
 * Subclasses should specify the XformConfig required to configure the shader as
 * well as supplying source code for the shaders themselves.
 */
#ifndef IMAGE_TOYS_SINGLE_IO_SHADER_XFORM_H
#define IMAGE_TOYS_SINGLE_IO_SHADER_XFORM_H

#include "render-xform.h"
#include "shader.h"

class SingleIOShaderXform : public RenderXform {
public:
  explicit SingleIOShaderXform(const std::string &name, XformConfig config);

  ~SingleIOShaderXform() override;

  virtual std::string type() const override = 0;

protected:
  void do_init_fbo() override;

private:
  virtual void init_shader(uint32_t &err, std::string &err_msg) = 0;

  virtual void bind_shader_variables(std::shared_ptr<Shader> shader) = 0;

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs, uint32_t &err, std::string &err_msg) override;

  GLuint texture_id_;
  std::shared_ptr<Shader> shader_;
};

#endif //IMAGE_TOYS_SINGLE_IO_SHADER_XFORM_H
