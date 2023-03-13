#include "split-channel-xform.h"

#include <spdlog/spdlog-inl.h>

uint32_t SplitChannelXform::next_idx_ = 0;

SplitChannelXform::SplitChannelXform() //
        : Xform("SplitChannel_" + std::to_string(next_idx_++),XformConfig()) //
{
  add_input_port_descriptor("image", "image");
  add_output_port_descriptor("red", "image");
  add_output_port_descriptor("green", "image");
  add_output_port_descriptor("blue", "image");
  add_output_port_descriptor("alpha", "image");
}

std::map<std::string, void *>
SplitChannelXform::do_apply(const std::map<std::string, void *> &inputs) {
  spdlog::info("Split");
  return {
          {"red", nullptr},
          {"green", nullptr},
          {"blue", nullptr},
          {"alpha", nullptr}
  };
}
