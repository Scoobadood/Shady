#include "save-file-xform.h"
#include <spdlog/spdlog-inl.h>

uint32_t SaveFileXform::next_idx_ = 0;

SaveFileXform::SaveFileXform() //
        : Xform("SaveFile_" + std::to_string(next_idx_++)) //
{
  add_input_port_descriptor("image", "image", true);
}

std::map<std::string, void *>
SaveFileXform::do_apply(const std::map<std::string, void *> &inputs) {
  spdlog::info("Saved file");
  return {
  };
}
