#ifndef IMAGE_TOYS_COMMAND_DELETE_H
#define IMAGE_TOYS_COMMAND_DELETE_H

#include "command.h"

class Delete : public CommandWithArgs {
public:
  explicit Delete(const std::vector<std::string> &args);

  ~Delete() override = default;

  int32_t execute(Context &context) override;

private:
  std::string xform_name_;
};

#endif //IMAGE_TOYS_COMMAND_DELETE_H
