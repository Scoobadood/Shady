#include "xforms/load-file-xform.h"
#include "image_io.h"
#include "gl_utils.h"
#include "xform-texture-meta.h"
#include "xform-factory.h"

#include "spdlog/spdlog-inl.h"

#include <utility>

std::string LoadFileXform::type() const {
  return TYPE;
}

namespace {
  const std::vector<const XformConfig::PropertyDescriptor> LOAD_FILE_PROPERTIES
          {{"file_name", XformConfig::PropertyDescriptor::STRING}};

}

LoadFileXform::LoadFileXform(const std::string &name) //
        : Xform(name  //
        , XformConfig(LOAD_FILE_PROPERTIES)) //
{
  image_tx_ = 0;
  add_output_port_descriptor("image", "image");
}

void LoadFileXform::init() {
  allocate_textures(1, &image_tx_);
  is_init_ = true;
}

LoadFileXform::~LoadFileXform() {
  glDeleteTextures(1, &image_tx_);
}

std::map<std::string, std::shared_ptr<void>>
LoadFileXform::do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
                        uint32_t &err, std::string &err_msg) {
  using namespace std;

  string file_name;
  if (!config().get("file_name", file_name)) {
    err = XFORM_MISSING_CONFIG;
    err_msg = fmt::format("Missing 'file_name'");
    return {};
  }

  int32_t width, height;
  uint8_t *image_bytes;
  if (IO_OK != load_image(file_name, width, height, image_bytes)) {
    err = XFORM_FILE_READ_FAILED;
    err_msg = fmt::format("Couldn't load file '{}'", file_name);
    return {};
  }

  // load file to texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, image_tx_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               width, height, 0,
               GL_RGB, GL_UNSIGNED_BYTE,
               image_bytes);
  gl_check_error_and_halt("load file to texture");
  free(image_bytes);

  spdlog::info("Loaded file from {} into txid {}", file_name, image_tx_);

  err = XFORM_OK;
  err_msg = "ok";
  return {
          {"image", static_pointer_cast<void>(make_shared<TextureMetadata>(image_tx_, width, height))}
  };
}
