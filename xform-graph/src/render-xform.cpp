#include "render-xform.h"
#include "gl_utils.h"

#include <spdlog/spdlog-inl.h>

#include <utility>

RenderXform::RenderXform(const std::string &name, XformConfig config) //
        : Xform(name, std::move(config)) //
        , fbo_{0} //
        , vao_id_{0}//
        , vbo_verts_{0}//
        , vbo_indices_{0}//
{
}

void RenderXform::init() {
  spdlog::debug("RenderXform::init");
  init_gl_resources();
}

RenderXform::~RenderXform() {
  glDeleteBuffers(1, &vbo_verts_);
  glDeleteBuffers(1, &vbo_indices_);
  glDeleteVertexArrays(1, &vao_id_);
  glDeleteFramebuffers(1, &fbo_);
}

void init_buffers(GLuint &vao_id, GLuint &vbo_verts, GLuint &vbo_indices) {
  spdlog::debug("RenderXform::init_buffers()");

  float vertex_data[4 * 4] = {-1.0, -1.0, 0, 0,
                              1.0, -1.0, 1, 0,
                              1.0, 1.0, 1, 1,
                              -1.0, 1.0, 0, 1
  };
  GLushort indices[] = {0, 1, 2, 0, 2, 3};

  glGenVertexArrays(1, &vao_id);
  glGenBuffers(1, &vbo_verts);
  glGenBuffers(1, &vbo_indices);
  glBindVertexArray(vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_verts);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), &vertex_data[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
  gl_check_error_and_halt("init_buffers()");
}


void RenderXform::init_framebuffer() {
  spdlog::debug("RenderXform::init_framebuffer()");
  glGenFramebuffers(1, &fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

  do_init_fbo();

  auto err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (err != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error(fmt::format("FB not complete : {}", err));
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderXform::init_gl_resources() {
  spdlog::debug("RenderXform::init_gl_resources()");
  init_framebuffer();
  init_buffers(vao_id_, vbo_verts_, vbo_indices_);
  gl_check_error_and_halt("init_gl_resources()");
}

void RenderXform::start_render() const{
  spdlog::debug("RenderXform::start_render()");

  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

  GLenum drawBuffers[num_textures_];
  for( GLenum i=0; i< num_textures_; ++i) drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
  glDrawBuffers(num_textures_, drawBuffers);

  glBindVertexArray(vao_id_);
}

void RenderXform::end_render(){
  spdlog::debug("RenderXform::end_render()");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}