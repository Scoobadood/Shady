/*
 * An XformConfig is a set of parameters which define how an Xform
 * works.
 */
#ifndef IMAGE_TOYS_XFORM_CONFIG_H
#define IMAGE_TOYS_XFORM_CONFIG_H

#include <string>
#include <map>
#include <utility>
#include <vector>

class XformConfig {
public:
  struct PropertyDescriptor {
    std::string name;
    enum Type {
      STRING,
      FLOAT,
      INT,
      UNKNOWN
    } type;

    bool operator==(const PropertyDescriptor &other) const {
      return other.type == type && other.name == name;
    }

    std::string type_name() const {
      switch(type) {
        case STRING: return "STRING";
        case FLOAT: return "FLOAT";
        case INT: return "INT";
        case UNKNOWN: return "UNKNOWN";
      }
    }
  };

  explicit XformConfig(const std::vector<const PropertyDescriptor> &descriptors = {});

  std::vector<const PropertyDescriptor> descriptors() const;

  PropertyDescriptor::Type type_for_property(const std::string& prop_name) const;

  void set(const std::string &name, const std::string &value);

  void set(const std::string &name, float value);

  void set(const std::string &name, int value);

  bool get(const std::string &name, std::string &value) const;

  bool get(const std::string &name, float &value) const;

  bool get(const std::string &name, int &value) const;

private:
  class XProperty {
  protected:
    explicit XProperty(std::string name) //
            : name_(std::move(name)) {}

  private:
    std::string name_;
  };

  template<typename T>
  class TypedProperty : public XProperty {
  public:
    TypedProperty(const std::string &name, T data) //
            : XProperty{name} //
            , value_{std::move(data)}//
    {}

    T value() const { return value_; }

  private:
    T value_;
  };

  std::map<std::string, const PropertyDescriptor> descriptors_;
  std::map<std::string, std::shared_ptr<XProperty> > values_;
};

#endif //IMAGE_TOYS_XFORM_CONFIG_H
