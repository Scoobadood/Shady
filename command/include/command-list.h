//
// Created by Dave Durbin on 17/3/2023.
//

#ifndef SHADY_COMMAND_LIST_H
#define SHADY_COMMAND_LIST_H

#include "command-root.h"


class List : public Command {
public:
  explicit List(const std::vector<std::string> &args) //
          : Command() //
  {}

  ~List() override = default;

  int32_t execute(Context &context) override;
};


#endif //SHADY_COMMAND_LIST_H
