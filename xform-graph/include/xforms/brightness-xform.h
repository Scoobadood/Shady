/*
 * Adjust brightness.
 * Parameters are  [-100, 100] which drive brightness from 0.5x to 2x of original
 */
#ifndef IMAGE_TOYS_BRIGHTNESS_XFORM_H
#define IMAGE_TOYS_BRIGHTNESS_XFORM_H

#include "single-io-shader-xform.h"
#include "xform-factory.h"

class BrightnessXform : public SingleIOShaderXform {
public:
  explicit BrightnessXform(const std::string &name);

  ~BrightnessXform();

  std::string type() const override;

private:
  static uint32_t next_idx_;

  virtual void init_shader(uint32_t &err, std::string &err_msg) override;

  virtual void bind_shader_variables(std::shared_ptr<Shader> shader) override;
};

REGISTER_CLASS(BrightnessXform)

#endif //IMAGE_TOYS_BRIGHTNESS_XFORM_H
