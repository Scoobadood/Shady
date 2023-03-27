//
// Created by Dave Durbin on 15/3/2023.
//

#ifndef IMAGE_TOYS_XFORM_FACTORY_H
#define IMAGE_TOYS_XFORM_FACTORY_H

#include "xforms/xform-config.h"
#include "xforms/xform.h"

using FactoryFn = std::function<std::shared_ptr<Xform>(const std::string &)>;

class XformFactory {
public:
  static std::shared_ptr<Xform>
  make_xform(const std::string &type);

  static std::shared_ptr<Xform>
  make_xform(const std::string &type,
             const std::string &name,
             const std::map<std::string, std::string> &conf);

  static bool can_make(const std::string &type) {
    return registry_.find(type) != registry_.end();
  }

  static void
  register_type(const std::string &type,
                const FactoryFn &creator);

  static std::vector<std::string>
  registered_types();

private:
  static std::map<std::string, FactoryFn> registry_;
};


#define REGISTER_XFORM(cls,type)                              \
std::shared_ptr<cls> create_##cls(const std::string& name) {  \
  std::shared_ptr<cls> c;                                     \
  if( name.empty())                                           \
    c = std::make_shared<cls>();                              \
  else                                                        \
    c = std::make_shared<cls>(name);                          \
  c->init();                                                  \
  return c;                                                   \
}                                                             \
                                                              \
struct Register##cls {                                        \
  explicit Register##cls(const std::string& name="") {        \
    XformFactory::register_type(name, &create_##cls);         \
  }                                                           \
};                                                            \
                                                              \
const std::string cls::TYPE = #type;                          \
static Register##cls myclass_##cls(cls::TYPE);


#endif //IMAGE_TOYS_XFORM_FACTORY_H
