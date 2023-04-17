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

uniform sampler2D input_image;
uniform float coeff[25];
uniform vec2 pixel_size;

void main() {
  vec2 hor=vec2(pixel_size.x, 0);
  vec2 ver=vec2(0, pixel_size.y);
  vec3 col = vec3(0);
  for( int i=0; i<5; ++i ) {
    for( int j=0; j<5; ++j ) {
      vec2 coord = uv_tex_coord.x + (hor * ( i - 2 )) + (ver * ( j - 2 ));
      col = col + texture(input_image, coord).rgb * coeff[(5*i)+j];
    }
  }
  fragColor=vec4(col , 1);
}
)"};
}


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

/*
 * 01 04 07 04 01
 * 04 16 26 16 04
 * 07 26 41 26 07
 * 04 16 26 16 04
 * 01 04 07 04 01
 *
 * 1/273
 */
void GaussianBlurXform::bind_shader_variables() {
  spdlog::debug("GaussianBlur::bind_shader_variables()");
  gl_check_error_and_halt("GaussianBlurXform::bind_shader_variables()");

  float coeff[25]{
          1. / 273., 4. / 273., 7. / 273., 4. / 273., 1. / 273.,
          4. / 273., 16. / 273., 26. / 273., 16. / 273., 4. / 273.,
          7. / 273., 26. / 273., 41. / 273., 26. / 273., 7. / 273.,
          4. / 273., 16. / 273., 26. / 273., 16. / 273., 4. / 273.,
          1. / 273., 4. / 273., 7. / 273., 4. / 273., 1. / 273.
  };
  shader_->set_floats("coeff", 25, coeff);
  shader_->set_vec2("pixel_size", glm::vec2(1.0f/453.0f, 1.0f/340.0f));
  gl_check_error_and_halt("GaussianBlurXform::set_vec2()");
}