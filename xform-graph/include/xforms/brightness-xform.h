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

  ~BrightnessXform() override;

  std::string type() const override;

  void init() override;


private:
  static uint32_t next_idx_;

  void init_shader() override;

  void bind_shader_variables() override;
};

REGISTER_CLASS(BrightnessXform)

#endif //IMAGE_TOYS_BRIGHTNESS_XFORM_H
