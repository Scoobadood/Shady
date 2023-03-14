/*
 Splits an image into RGBA channels.

 InputPort:
    Image: RGBAImage

 OutputPorts:
    Red : GreyImage
    Green : GreyImage
    Blue : GreyImage
    Alpha : GreyImage

 If the input image has four channels thsi works as expected.
 If only RGB then Alpha will be 1.0, 0.0, max (of others), min (of others), mean(of others), red, green or blue
 as specified in the configuration.
 If the input has only a single channel then it will be copied to all output channels.
 */
#ifndef IMAGE_TOYS_SPLIT_CHANNEL_XFORM_H
#define IMAGE_TOYS_SPLIT_CHANNEL_XFORM_H

#include "gl_utils.h"
#include "shader.h"
#include "render-xform.h"

#include <OpenGL/gl3.h>

class SplitChannelXform : public RenderXform {
public:
  SplitChannelXform();

  ~SplitChannelXform() override;

protected:
  static uint32_t next_idx_;

private:
  GLuint texture_ids_[4]; // R G B A

  void do_init_fbo() override;

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string &err_msg) override;

  std::shared_ptr<Shader> split_prog_;
};

#endif //IMAGE_TOYS_SPLIT_CHANNEL_XFORM_H