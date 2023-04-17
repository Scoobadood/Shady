/*
 * Apply a Gaussian Blur
 * We need to manage the kernel size and the sigma as a proportion of the
 * image size.
 * For now, we'll just have a fixed value of both, a 5x5 kernel with sigma 2
 */
#ifndef SHADY_GAUSSIAN_BLUR_XFORM_H
#define SHADY_GAUSSIAN_BLUR_XFORM_H

#include "single-io-shader-xform.h"
#include "xform-factory.h"

class GaussianBlurXform : public SingleIOShaderXform {
public:
  explicit GaussianBlurXform(const std::string &name = "GaussianBlur");

  ~GaussianBlurXform() override;

  std::string type() const override;

  const static std::string TYPE;

  void init() override;

  bool is_config_valid() const override {
    return true;
  };

private:
  void init_shader() override;

  void bind_shader_variables() override;
};

REGISTER_XFORM(GaussianBlurXform, Gaussian)

#endif
