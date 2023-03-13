#include "save-file-xform.h"
#include <spdlog/spdlog-inl.h>

uint32_t SaveFileXform::next_idx_ = 0;

SaveFileXform::~SaveFileXform() = default;

SaveFileXform::SaveFileXform() //
        : Xform("SaveFile_" + std::to_string(next_idx_++),
                XformConfig{
                        {
                                {"file_name", XformConfig::PropertyDescriptor::STRING}}
                }
) //
{
  add_input_port_descriptor("image", "image", true);
}

std::map<std::string, std::shared_ptr<void>>
SaveFileXform::do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs, uint32_t &err, std::string &err_msg) {
  std::string file_name;
  if (!config().get("file_name", file_name)) {
    spdlog::error("No file name specified for {}", name());
    return {};
  }

  spdlog::info("Saved file {}", file_name);
  return {
  };
}
