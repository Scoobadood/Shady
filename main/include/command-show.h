#ifndef IMAGE_TOYS_COMMAND_SHOW_H
#define IMAGE_TOYS_COMMAND_SHOW_H

#include "command.h"

class Show : public CommandWithArgs {
public:
  explicit Show(const std::vector<std::string> &args);

  ~Show() override = default;

  int32_t execute(std::map<std::string, std::shared_ptr<void>> &context) override;

private:
  std::string xform_name_;
};

#endif //IMAGE_TOYS_COMMAND_SHOW_H
