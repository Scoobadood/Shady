/*
 * Adjust brightness.
 * Parameters are  [-100, 100] which drive brightness from 0.5x to 2x of original
 */
#ifndef SHADY_BRIGHTNESS_XFORM_H
#define SHADY_BRIGHTNESS_XFORM_H

#include "single-io-shader-xform.h"
#include "xform-factory.h"

class BrightnessXform : public SingleIOShaderXform {
public:
  explicit BrightnessXform(const std::string &name = "Brightness");

  ~BrightnessXform() override;

  std::string type() const override;

  const static std::string TYPE;

  void init() override;

  bool is_config_valid() const override {
    int br;
    return config().get("brightness", br);
  };

private:
  void init_shader() override;

  void bind_shader_variables() override;
};

REGISTER_XFORM(BrightnessXform, Brightness)

#endif //SHADY_BRIGHTNESS_XFORM_H
