#include "load-file-xform.h"
#include <spdlog/spdlog-inl.h>

uint32_t LoadFileXform::next_idx_ = 0;

LoadFileXform::LoadFileXform() //
        : Xform("LoadFile_" + std::to_string(next_idx_++)) //
{
  add_output_port_descriptor("image", "image");
}

std::map<std::string, void *>
LoadFileXform::do_apply(const std::map<std::string, void *> &inputs) {
  spdlog::info("Loaded file");
  return {
          {"image", nullptr}
  };
}
