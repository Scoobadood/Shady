#ifndef SHADY_ADD_XFORM_H
#define SHADY_ADD_XFORM_H

/*
 Adds two channels into a single RGBA image.

 InputPort:
    RGBA image 1
    RGBA image 2

 OutputPorts:
    Image: RGBAImage
 */
#include "xform-factory.h"
#include "gl_utils.h"
#include "shader.h"
#include "render-xform.h"

#include <OpenGL/gl3.h>

class AddChannelXform : public RenderXform {
public:
  explicit AddChannelXform(const std::string &name = "AddChannel");

  ~AddChannelXform() override;

  std::string type() const override;

  static const std::string TYPE;

  void init() override;

  bool is_config_valid() const override {
    // No config
    return true;
  }


private:
  GLuint texture_id_;

  void configure_framebuffer() override;

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string &err_msg) override;

  std::shared_ptr<Shader> add_prog_;
};

REGISTER_XFORM(AddChannelXform, AddChannel)

#endif
