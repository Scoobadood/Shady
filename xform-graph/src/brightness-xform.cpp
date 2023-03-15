#include "xforms/brightness-xform.h"


namespace {
  const GLchar *v_shader_source[] = {R"(
#version 410 core
layout(location=0) in vec2 vtx_coord;
layout(location=1) in vec2 vtx_tex_coord;
smooth out vec2 uv_tex_coord;
void main() {
  gl_Position = vec4(vtx_coord,0,1);
  uv_tex_coord = vtx_tex_coord;
})"
  };

  const GLchar *f_shader_source[] = {R"(
#version 410 core
layout (location=0) out vec4 fragColor[4];
smooth in vec2 uv_tex_coord;
uniform sampler2D input_image;
uniform float brightness;

void main() {
  vec4 src = texture( input_image, uv_tex_coord );
  fragColor[0] = vec4(src.rgb * brightness, src.a);
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
  config().set("brightness", 0);
}

BrightnessXform::~BrightnessXform() = default;

void BrightnessXform::init_shader() {
  shader_ = std::make_shared<Shader>(v_shader_source,
                                     nullptr,
                                     f_shader_source);
}

void BrightnessXform::init() {
  SingleIOShaderXform::init();
  is_init_ = (shader_ != nullptr);
}

void BrightnessXform::bind_shader_variables(std::shared_ptr<Shader> shader) {
  int br;
  config().get("brightness", br);
  br = std::min(100, std::max(br, -100));
  //-100 -> 0.5, 100 = 2.0, 0 -> 0
  float fbr = (br < 0) ? ((float) br / -200) : ((float) br / 50);
  shader->set1f("brightness", fbr);
}
