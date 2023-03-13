#ifndef IMAGE_TOYS_LOAD_FILE_XFORM_H
#define IMAGE_TOYS_LOAD_FILE_XFORM_H

#include "xform.h"

class LoadFileXform : public Xform {
public:
  LoadFileXform();

  ~LoadFileXform() override;

protected:
  static uint32_t next_idx_;

private:
  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs, uint32_t &err, std::string &err_msg) override;

  GLuint image_tx_;
};

#endif //IMAGE_TOYS_LOAD_FILE_XFORM_H
