#include "xforms/xform-config.h"
#include <spdlog/spdlog-inl.h>

#include <utility>
#include <vector>
#include <map>

XformConfig::XformConfig(const std::vector<const PropertyDescriptor> &descriptors) {
  for (const auto &pd: descriptors) {
    descriptors_.emplace(pd.name, pd);
  }
}

std::vector<const XformConfig::PropertyDescriptor>
XformConfig::descriptors() const {
  std::vector<const PropertyDescriptor> pds;
  for (const auto &pd: descriptors_) {
    pds.push_back(pd.second);
  }
  return pds;
}

void
XformConfig::set(const std::string &name, const std::string &value) {
  auto it = descriptors_.find(name);
  if (it == descriptors_.end()) return;
  if (it->second.type != PropertyDescriptor::STRING) return;

  std::shared_ptr<TypedProperty<const std::string>> xp = std::make_shared<TypedProperty<const std::string>>(name, value);

  auto it2 = values_.find(name);
  if (it2 != values_.end()) {
    auto ptr = std::static_pointer_cast<TypedProperty<std::string>>(it2->second);
    spdlog::info("Old value : {}", ptr->value());
  }
  values_[name]= xp;

  it2 = values_.find(name);
  auto ptr = std::static_pointer_cast<TypedProperty<std::string>>(it2->second);
  spdlog::info("New value : {}", ptr->value());


}

void
XformConfig::set(const std::string &name, float value) {
  auto it = descriptors_.find(name);
  if (it == descriptors_.end()) return;
  if (it->second.type != PropertyDescriptor::FLOAT) return;

  auto xp = std::make_shared<TypedProperty<float>>(name, value);
  values_[name]= xp;
}

void
XformConfig::set(const std::string &name, int value) {
  auto it = descriptors_.find(name);
  if (it == descriptors_.end()) return;
  if (it->second.type != PropertyDescriptor::INT) return;

  auto xp = std::make_shared<TypedProperty<int>>(name, value);
  values_[name]= xp;
}

bool
XformConfig::get(const std::string &name, std::string &value) const {
  auto it = descriptors_.find(name);
  if (it == descriptors_.end()) return false;
  if (it->second.type != PropertyDescriptor::STRING) return false;

  auto it2 = values_.find(name);
  if (it2 == values_.end()) return false;
  auto ptr = std::static_pointer_cast<TypedProperty<std::string>>(it2->second);
  if (!ptr) return false;

  value = ptr->value();
  return true;
}

bool
XformConfig::get(const std::string &name, float &value) const {
  auto it = descriptors_.find(name);
  if (it == descriptors_.end()) return false;
  if (it->second.type != PropertyDescriptor::FLOAT) return false;

  auto it2 = values_.find(name);
  if (it2 == values_.end()) return false;
  auto ptr = std::static_pointer_cast<TypedProperty<float>>(it2->second);
  if (!ptr) return false;

  value = ptr->value();
  return true;
}

bool
XformConfig::get(const std::string &name, int &value) const {
  auto it = descriptors_.find(name);
  if (it == descriptors_.end()) return false;
  if (it->second.type != PropertyDescriptor::INT) return false;

  auto it2 = values_.find(name);
  if (it2 == values_.end()) return false;
  auto ptr = std::static_pointer_cast<TypedProperty<int>>(it2->second);
  if (!ptr) return false;

  value = ptr->value();
  return true;
}

XformConfig::PropertyDescriptor::Type
XformConfig::type_for_property(const std::string& prop_name) const {
  auto it = descriptors_.find(prop_name);
  if( it == descriptors_.end())
    return PropertyDescriptor::UNKNOWN;
  return it->second.type;
}
