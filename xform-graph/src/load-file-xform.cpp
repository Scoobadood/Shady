#include "load-file-xform.h"
#include <spdlog/spdlog-inl.h>

uint32_t LoadFileXform::next_idx_ = 0;


LoadFileXform::~LoadFileXform(){}


LoadFileXform::LoadFileXform() //
        : Xform("LoadFile_" + std::to_string(next_idx_++),
                XformConfig({{"file_name", XformConfig::PropertyDescriptor::STRING}})
        ) //
{
  add_output_port_descriptor("image", "image");
}

std::map<std::string, void *>
LoadFileXform::do_apply(const std::map<std::string, void *> &inputs) {
  std::string file_name;
  if( !config().get("file_name", file_name)) {
    spdlog::error( "No file name specified for {}", name());
    return {};
  }

  // load file to texture
  spdlog::info("Loaded file from {}", file_name);

  return {
          {"image", nullptr}
  };
}
