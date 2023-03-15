#ifndef IMAGE_TOYS_LOAD_FILE_XFORM_H
#define IMAGE_TOYS_LOAD_FILE_XFORM_H

#include "xform.h"
#include "xform-factory.h"

class LoadFileXform : public Xform {
public:
  LoadFileXform();

  explicit LoadFileXform(const std::string& name);

  ~LoadFileXform() override;

  std::string type() const override;

  /**
   * Virtual initialisation; MUST be called before object can be used.
   */
  void init() override;

private:
  static uint32_t next_idx_;

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string &err_msg) override;

  /* Texture to load image to */
  GLuint image_tx_;
};

REGISTER_CLASS(LoadFileXform)

#endif //IMAGE_TOYS_LOAD_FILE_XFORM_H
