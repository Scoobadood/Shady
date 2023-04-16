#ifndef SHADY_COMMAND_QUIT_H
#define SHADY_COMMAND_QUIT_H

#include "command-root.h"

class Quit : public Command {
public:
  int32_t execute(Context &context) override;

  ~Quit() override = default;
};



#endif //SHADY_COMMAND_QUIT_H
