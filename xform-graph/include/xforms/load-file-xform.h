#ifndef SHADY_LOAD_FILE_XFORM_H
#define SHADY_LOAD_FILE_XFORM_H

#include "xform.h"
#include "xform-factory.h"

class LoadFileXform : public Xform {
public:
  explicit LoadFileXform(const std::string &name = "LoadFile");

  ~LoadFileXform() override;

  std::string type() const override;

  const static std::string TYPE;

  /**
   * Virtual initialisation; MUST be called before object can be used.
   */
  void init() override;

  bool is_config_valid() const override {
    std::string fn;
    return config().get("file_name", fn);
  }

private:
  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string
           &err_msg) override;

  /* Texture to load image to */
  GLuint image_tx_;
};

REGISTER_XFORM(LoadFileXform, LoadFile)

#endif //SHADY_LOAD_FILE_XFORM_H
