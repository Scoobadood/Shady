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

#include "xform.h"

class SplitChannelXform : public Xform {
public:
  SplitChannelXform();

protected:
  static uint32_t next_idx_;

private:
  std::map<std::string, void *>
  do_apply(const std::map<std::string, void *> &inputs) override;
};

#endif //IMAGE_TOYS_SPLIT_CHANNEL_XFORM_H
