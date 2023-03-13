#ifndef IMAGE_TOYS_SAVE_FILE_XFORM_H
#define IMAGE_TOYS_SAVE_FILE_XFORM_H

#include "xform.h"

class SaveFileXform : public Xform {
public:
  SaveFileXform();

protected:
  static uint32_t next_idx_;

private:
  std::map<std::string, void *>
  do_apply(const std::map<std::string, void *> &inputs) override;
};

#endif //IMAGE_TOYS_SAVE_FILE_XFORM_H
