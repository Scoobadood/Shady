#include "xforms/gaussian-blur-xform.h"
#include "spdlog/spdlog-inl.h"
#include "gl_utils.h"

namespace {
  const GLchar *v_shader_source[] = {R"(
#version 410 core
layout(location=0) in vec2 vtx_coord;
layout(location=1) in vec2 vtx_tex_coord;

out vec2 uv_tex_coord;

void main() {
  gl_Position = vec4(vtx_coord,0,1);
  uv_tex_coord = vtx_tex_coord;
})"};

  const GLchar *f_shader_source[] = {R"(
#version 410 core
layout (location=0) out vec4 fragColor;

in vec2 uv_tex_coord;

const int KERNEL_RADIUS = 17;
const int KERNEL_SIZE = KERNEL_RADIUS * 2 + 1;
uniform sampler2D input_image;
uniform float coeff[KERNEL_SIZE];
uniform vec2 pixel_size;
uniform bool horizontal_pass;

void main() {
  vec2 offset = (horizontal_pass)
    ? vec2(pixel_size.x, 0)
    : vec2(0, pixel_size.y);
  vec3 col = vec3(0);
  for( int i=-KERNEL_RADIUS; i<=KERNEL_RADIUS; ++i ) {
      vec2 coord = uv_tex_coord + offset * i;
      col = col + texture(input_image, coord).rgb * coeff[i + KERNEL_RADIUS];
  }
  fragColor=vec4(col , 1);
}
)"};
}

const int32_t MAX_KERNEL_RADIUS = 17;

std::string GaussianBlurXform::type() const {
  return TYPE;
}

static const std::vector<const XformConfig::PropertyDescriptor> GAUSSIAN_BLUR_PROPERTIES{
        {"sigma", XformConfig::PropertyDescriptor::FLOAT}
};

GaussianBlurXform::GaussianBlurXform(const std::string &name) //
        : SingleIOShaderXform(name  //
        , XformConfig(GAUSSIAN_BLUR_PROPERTIES)) //
{
  config().set("sigma", 2.4f);
}

GaussianBlurXform::~GaussianBlurXform() = default;

void GaussianBlurXform::init_shader() {
  spdlog::debug("GaussianBlur::init_shader()");

  shader_ = std::unique_ptr<Shader>(new Shader(v_shader_source,
                                               f_shader_source));
}

void GaussianBlurXform::init() {
  spdlog::debug("GaussianBlur::init()");
  SingleIOShaderXform::init();
  is_init_ = (shader_ != nullptr);
}

void compute_kernel(int32_t kernel_radius, float* kernel, float sigma) {
  auto b = 1.0f / (sqrtf( 2.0f * M_PI ) * sigma);
  float total = 0.0f;
  for( auto i = -kernel_radius; i<= kernel_radius; ++i) {
    kernel[i+kernel_radius] =  b * exp(-(i*i) / (2.0f * sigma * sigma));
    total +=kernel[i+kernel_radius];
  }
  for( auto i = -kernel_radius; i<= kernel_radius; ++i) {
    kernel[i+kernel_radius] /= total;
  }
}

void GaussianBlurXform::bind_shader_variables() {
  spdlog::debug("GaussianBlur::bind_shader_variables()");
  gl_check_error_and_halt("GaussianBlurXform::bind_shader_variables()");

  float sigma;
  config().get("sigma", sigma);

  float coeff[MAX_KERNEL_RADIUS * 2 + 1];
  compute_kernel(MAX_KERNEL_RADIUS, coeff, sigma);

  shader_->set_floats("coeff", MAX_KERNEL_RADIUS * 2 + 1, coeff);
  shader_->set_vec2("pixel_size", glm::vec2(1.0f/453.0f, 1.0f/340.0f));
  gl_check_error_and_halt("GaussianBlurXform::set_vec2()");
}