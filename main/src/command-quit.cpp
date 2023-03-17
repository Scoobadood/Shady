#include "command-quit.h"

int32_t Quit::execute(Context &context) {
  context["should_quit"] = std::make_shared<bool>(true);
  return CMD_NO_ERROR;
}