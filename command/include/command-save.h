#ifndef SHADY_COMMAND_SAVE_H
#define SHADY_COMMAND_SAVE_H

#include "command-root.h"

class Save : public CommandWithArgs {
public:
  explicit Save(const std::vector<std::string> &args);

  ~Save() override = default;

  int32_t execute(Context &context) override;

private:
  std::string file_name_;
};

#endif //SHADY_COMMAND_SAVE_H
