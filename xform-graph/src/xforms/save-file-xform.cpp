#include "xforms/save-file-xform.h"
#include "spdlog/spdlog-inl.h"

#include <utility>
#include "xform-texture-meta.h"
#include "image_io.h"

uint32_t SaveFileXform::next_idx_ = 0;

std::string SaveFileXform::type() const {
  return TYPE;
}

void save_texture_to_file(const std::string &file_name,
                          const std::shared_ptr<TextureMetadata> &tx_data,
                          uint32_t &err,
                          std::string &err_msg);

namespace {
  const std::vector<const XformConfig::PropertyDescriptor> SAVE_FILE_PROPERTIES
          {{"file_name", XformConfig::PropertyDescriptor::STRING}};

}

SaveFileXform::SaveFileXform(std::string name) //
        : Xform(std::move(name) //
        , XformConfig{SAVE_FILE_PROPERTIES}) //
{
  add_input_port_descriptor("image", "image", true);
}

void SaveFileXform::init() {
  is_init_ = true;
}

SaveFileXform::SaveFileXform() :
        SaveFileXform(fmt::format("{}_{}", TYPE, next_idx_++)) {}

SaveFileXform::~SaveFileXform() = default;

std::map<std::string, std::shared_ptr<void>>
SaveFileXform::do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs, uint32_t &err,
                        std::string &err_msg) {
  using namespace std;

  string file_name;
  if (!config().get("file_name", file_name)) {
    err = XFORM_MISSING_CONFIG;
    err_msg = fmt::format("Missing 'file_name'");
    return {};
  }

  auto img = std::static_pointer_cast<TextureMetadata>(inputs.at("image"));
  if (!img) {
    err = XFORM_INPUT_NOT_SET;
    err_msg = fmt::format("'image' is null");
    return {};
  }

  save_texture_to_file(file_name, img, err, err_msg);

  spdlog::info("Saved file {}", file_name);
  return {
  };
}

void save_texture_to_file(const std::string &file_name,
                          const std::shared_ptr<TextureMetadata> &tx_data,
                          uint32_t &err,
                          std::string &err_msg) {

  auto buffer = new uint8_t[tx_data->width * tx_data->height * 4];
  glBindTexture(GL_TEXTURE_2D, tx_data->texture_id);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

  auto x = save_png(file_name, tx_data->width, tx_data->height, buffer);
  delete[] buffer;

  if (x == IO_OK) {
    err = XFORM_OK;
    err_msg = "OK";
  } else {
    err = XFORM_FILE_SAVE_FAILED;
    err_msg = fmt::format("Failed to write to {}", file_name);
  }
}
