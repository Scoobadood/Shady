/*
 Merges multiple (up to 4) channels into a singvle RGBA image.

 InputPort:
 One or more of :
    Red : GreyImage
    Green : GreyImage
    Blue : GreyImage
    Alpha : GreyImage

 OutputPorts:
    Image: RGBAImage

 If all four inputs are present, this works as expected.
 For any missing channel, we assume 0 values.
 */
#ifndef IMAGE_TOYS_MERGE_CHANNEL_XFORM_H
#define IMAGE_TOYS_MERGE_CHANNEL_XFORM_H

#include "xform-factory.h"
#include "gl_utils.h"
#include "shader.h"
#include "render-xform.h"

#include <OpenGL/gl3.h>

class MergeChannelXform : public RenderXform {
public:
  explicit MergeChannelXform(const std::string &name);

  MergeChannelXform();

  ~MergeChannelXform() override;

  std::string type() const override;

  static const std::string TYPE;

  void init() override;

  bool is_config_valid() const override {
    // No config
    return true;
  }


private:
  static uint32_t next_idx_;
  GLuint texture_id_;

  void configure_framebuffer() override;

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string &err_msg) override;

  std::shared_ptr<Shader> merge_prog_;
};

REGISTER_XFORM(MergeChannelXform, MergeChannel)


#endif //IMAGE_TOYS_MERGE_CHANNEL_XFORM_H