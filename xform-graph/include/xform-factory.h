//
// Created by Dave Durbin on 15/3/2023.
//

#ifndef IMAGE_TOYS_XFORM_FACTORY_H
#define IMAGE_TOYS_XFORM_FACTORY_H

#include "xform-config.h"
#include "xform.h"

using FactoryFn = std::function<std::shared_ptr<Xform>(const std::string &)>;

class XformFactory {
public:
  static std::shared_ptr<Xform>
  make_xform(const std::string &type,
             const std::string &name,
             const std::map<std::string, std::string> &conf);

  static void
  register_type(const std::string &type,
                const FactoryFn &creator);

private:
  static std::map<std::string, FactoryFn> registry_;
};


#define REGISTER_CLASS(cls)                                   \
std::shared_ptr<cls> create_##cls(const std::string& name) {  \
  return std::make_shared<cls>(name);                         \
}                                                             \
struct Register##cls {                                        \
  explicit Register##cls(const std::string& name) {           \
  XformFactory::register_type(name, &create_##cls);           \
}                                                             \
};                                                            \
static Register##cls myclass_##cls(#cls);


#endif //IMAGE_TOYS_XFORM_FACTORY_H
