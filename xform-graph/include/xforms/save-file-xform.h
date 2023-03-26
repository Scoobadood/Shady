#ifndef IMAGE_TOYS_SAVE_FILE_XFORM_H
#define IMAGE_TOYS_SAVE_FILE_XFORM_H

#include "xform.h"
#include "xform-factory.h"

class SaveFileXform : public Xform {
public:
  SaveFileXform();

  explicit SaveFileXform(std::string name);

  ~SaveFileXform() override;

  std::string type() const override;

  const static std::string TYPE;

  void init() override;

  bool is_config_valid() const override {
    std::string fn;
    return config().get("file_name", fn);
  }

private:
  static uint32_t next_idx_;

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string &err_msg) override;
};

REGISTER_XFORM(SaveFileXform, SaveFile)

#endif //IMAGE_TOYS_SAVE_FILE_XFORM_H
