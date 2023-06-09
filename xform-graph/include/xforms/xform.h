/*
 * A transformation.
 * Takes one or more inputs of various image types and operates on them to produce one or more outputs.
 * The inputs may have limitation on certain characteristics of the image
 * such as (colour depth, number of channels, dimensions etc.)
 * Some transformations are sources; they have no inputs but produce a single output.
 * Some are Sinks; they consume one or more inputs but produce no output.
 * Some may have multiple inputs, others multiple outputs.
 * They all have a Configuration which consists of a set of name value parameter pairs.
 */
#ifndef SHADY_XFORM_H
#define SHADY_XFORM_H

#include "xform-port.h"
#include "xform-config.h"

#include <string>
#include <map>
#include <vector>

#include <OpenGL/gl3.h>

const uint32_t XFORM_OK = 0;
const uint32_t XFORM_MISSING_CONFIG = 1;
const uint32_t XFORM_INVALID_CONFIG = 2;
const uint32_t XFORM_MISSING_INPUT = 3;
const uint32_t XFORM_INPUT_NOT_SET = 4;
const uint32_t XFORM_FILE_READ_FAILED = 5;
const uint32_t XFORM_FILE_SAVE_FAILED = 6;
const uint32_t XFORM_NOT_INITED = 7;
const uint32_t XFORM_NULL_INPUT  = 8;
const uint32_t XFORM_MISMATCHED_SIZE = 9;
const uint32_t XFORM_ALREADY_EXISTS = 10;

class Xform {
public:
  /**
   * Virtual initialisation; MUST be called before object can be used.
   */
  virtual void init() = 0;

  /**
   * Apply the transform to its inputs and make the result
   * available to outputs.
   * @param inputs A map from input port names to inputs.
   * @param err Error code populated in the event of a failure. Successs has XFROM_OK
   * @param err_msg An explanatory error message.
   * @return a map from output port names to outputs.
   */
  std::map<std::string, std::shared_ptr<void>>
  apply(const std::map<std::string, std::shared_ptr<void>> &inputs, uint32_t &err, std::string &err_msg);

  /**
   * @return a mutable reference to the config.
   */
  XformConfig &config() { return config_; }

  /**
   * @return an immutable reference to the config.
   */
  const XformConfig &config() const { return config_; }

  /**
   * @return The unique name for this transform.
   */
  const std::string &name() const;

  /**
   * @return The type for this transform.
   */
  virtual std::string type() const = 0;

  /**
   * @return true if there are no outputs for this xform.
   */
  bool inline is_sink() const {
    return output_port_descriptors_.empty();
  }

  /**
   * @return true if there are no inputs for this xform.
   */
  bool inline is_source() const {
    return input_port_descriptors_.empty();
  }

  /**
   * @return the InputPortsDescriptors for this xform
   */
  std::vector<std::shared_ptr<const InputPortDescriptor>>
  input_port_descriptors() const;

  /**
   * @return the OutputPortsDescriptors for this xform
   */
  std::vector<std::shared_ptr<const OutputPortDescriptor>>
  output_port_descriptors() const;

  /**
   * @param port_name
   * @return The InputPortDescriptor for the given port or nullptr if not found.
   */
  std::shared_ptr<const InputPortDescriptor>
  input_port_descriptor_for_port(const std::string &port_name) const;

/**
 * @param port_name
 * @return The OutputPortDescriptor for the given port or nullptr if not found.
 */
  std::shared_ptr<const OutputPortDescriptor>
  output_port_descriptor_for_port(const std::string &port_name) const;

  /**
   * Checks the validity of the config. This should return true if 'in principle' the
   * xform could be applied. i.e. if it needs a file_name, one has been provided and
   * has the right format.  The existence of the file is NOT a requirement for the
   * xform to be well-configured as this has the potential to change.
   *
   * @return true if the Configuration for this xform is valid.
   */
  virtual bool is_config_valid() const { return false; };

protected:
  /*
   * Construct from a subclass.
   */
  explicit Xform(std::string name, XformConfig config = XformConfig{});

  virtual ~Xform() = default;

  static void allocate_textures(int32_t num_textures, GLuint *texture_ids);

  static void resize_textures(int32_t num_textures, GLuint *texture_ids, GLsizei width, GLsizei height);

  void add_input_port_descriptor(const std::string &name, const std::string &type, bool is_required = true);

  void add_output_port_descriptor(const std::string &name, const std::string &type);

  std::map<std::string, std::shared_ptr<const InputPortDescriptor>> input_port_descriptors_;
  std::map<std::string, std::shared_ptr<const OutputPortDescriptor>> output_port_descriptors_;

  /* If true, the Xform can be applied */
  bool is_init_;

private:
  bool check_init(uint32_t &err, std::string &err_msg);

  bool check_required_inputs(const std::map<std::string, std::shared_ptr<void>> &inputs,
                             uint32_t &err, std::string &err_msg);

  /*
   * The core functionality of the shader should be implemented here.
   */
  virtual std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs, uint32_t &err, std::string &err_msg) = 0;

  std::string name_;

  XformConfig config_;
};


#endif //SHADY_XFORM_H
