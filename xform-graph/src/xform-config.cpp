#include "xform-config.h"

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

  auto xp = std::make_shared<TypedProperty<const std::string>>(name, value);
  values_.emplace(name, xp);
}

void
XformConfig::set(const std::string &name, float value) {
  auto it = descriptors_.find(name);
  if (it == descriptors_.end()) return;
  if (it->second.type != PropertyDescriptor::FLOAT) return;

  auto xp = std::make_shared<TypedProperty<float>>(name, value);
  values_.emplace(name, xp);
}

void
XformConfig::set(const std::string &name, int value) {
  auto it = descriptors_.find(name);
  if (it == descriptors_.end()) return;
  if (it->second.type != PropertyDescriptor::INT) return;

  auto xp = std::make_shared<TypedProperty<int>>(name, value);
  values_.emplace(name, xp);
}

bool
XformConfig::get(const std::string &name, std::string &value) {
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
XformConfig::get(const std::string &name, float &value) {
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
XformConfig::get(const std::string &name, int &value) {
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
