#include "xform-factory.h"

#include <string>
#include "spdlog/spdlog.h"

std::map<std::string, FactoryFn> XformFactory::registry_{};

std::shared_ptr<Xform>
XformFactory::make_xform(const std::string &type, const std::string &name,
           const std::map<std::string, std::string> &conf){
  auto it = registry_.find(type);
  if( it == registry_.end()) {
    spdlog::error( "Unrecognised xform type {}.", type);
    return nullptr;
  }

  std::shared_ptr<Xform> xform = it->second(name);

  for (const auto & e: conf) {
    switch (xform->config().type_for_property(e.first)) {
      case XformConfig::PropertyDescriptor::FLOAT:
        xform->config().set(e.first, stof(e.second));
        break;
      case XformConfig::PropertyDescriptor::INT:
        xform->config().set(e.first, stoi(e.second));
        break;
      case XformConfig::PropertyDescriptor::STRING:
        xform->config().set(e.first, e.second);
        break;
      case XformConfig::PropertyDescriptor::UNKNOWN:
        // Ignore
        break;
    }
  }
  return xform;
}

void
XformFactory::register_type(const std::string &type,
              const std::function<std::shared_ptr<Xform>(const std::string &)> &creator) {
  assert(creator);
  if( registry_.find(type) != registry_.end()) {
    spdlog::error( "Class {} is already registered.", type);
  }
  registry_.emplace(type, creator);
}
