#include "xforms/brightness-xform.h"
#include <spdlog/spdlog-inl.h>

namespace {
  const GLchar *v_shader_source[] = {R"(
#version 410 core
layout(location=0) in vec2 vtx_coord;
layout(location=1) in vec2 vtx_tex_coord;
smooth out vec2 uv_tex_coord;
void main() {
  gl_Position = vec4(vtx_coord,0,1);
  uv_tex_coord = vtx_tex_coord;
})"};

  const GLchar *f_shader_source[] = {R"(
#version 410 core
layout (location=0) out vec4 fragColor;
smooth in vec2 uv_tex_coord;
uniform float brightness;
uniform sampler2D input_image;

void main() {
  vec3 col = texture(input_image, uv_tex_coord).rgb * brightness;
  fragColor=vec4( col, 1.);
}
)"};
}


const std::string TYPE = "Brightness";
uint32_t BrightnessXform::next_idx_ = 0;

std::string BrightnessXform::type() const {
  return TYPE;
}

static const std::vector<const XformConfig::PropertyDescriptor> BRIGHTNESS_PROPERTIES{
        {"brightness", XformConfig::PropertyDescriptor::INT}
};

BrightnessXform::BrightnessXform(const std::string &name) //
        : SingleIOShaderXform(name  //
        , XformConfig(BRIGHTNESS_PROPERTIES)) //
{
  config().set("brightness", 100);
}

BrightnessXform::BrightnessXform() //
        : BrightnessXform(fmt::format("{}_{}", TYPE, next_idx_++))  //
{
  config().set("brightness", 0);
}

BrightnessXform::~BrightnessXform() = default;

void BrightnessXform::init_shader() {
  spdlog::debug("Brightness::init_shader()");

  shader_ = std::unique_ptr<Shader>(new Shader(v_shader_source,
                                               nullptr,
                                               f_shader_source));
}

void BrightnessXform::init() {
  spdlog::debug("Brightness::init()");
  SingleIOShaderXform::init();
  is_init_ = (shader_ != nullptr);
}

/*
 * Input values are in the range (-100, 100)
 * and map to a brightness multiplier as :
 *    -100 => 0.5
 *    0    => 1.0
 *    100  => 2.0
 */
void BrightnessXform::bind_shader_variables() {
  spdlog::debug("Brightness::bind_shader_variables()");

  int br;
  config().get("brightness", br);
  br = std::min(100, std::max(br, -100));

  float fbr;
  if (br < 0) fbr = (float) br / -200.0f;
  else if (br > 0) fbr = (float) br / 50.0f;
  else fbr = 1.0f;

  shader_->set1f("brightness", fbr);
}