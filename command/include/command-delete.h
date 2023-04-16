#ifndef SHADY_COMMAND_DELETE_H
#define SHADY_COMMAND_DELETE_H

#include "command-root.h"

class Delete : public CommandWithArgs {
public:
  explicit Delete(const std::vector<std::string> &args);

  ~Delete() override = default;

  int32_t execute(Context &context) override;

private:
  std::string xform_name_;
};

#endif //SHADY_COMMAND_DELETE_H
