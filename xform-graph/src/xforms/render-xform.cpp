#include "xforms/render-xform.h"
#include "gl_utils.h"

#include "spdlog/spdlog-inl.h"

#include <utility>

// Sets up a single quad
void init_buffers(GLuint &vao_id, GLuint &vbo_verts, GLuint &vbo_indices);


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

/*
 * Called in init(). Allocates resources used by this Xform.
 * - FBO
 * - VBO/VAO
 */
void RenderXform::init_gl_resources() {
  spdlog::debug("RenderXform::init_gl_resources()");
  init_buffers(vao_id_, vbo_verts_, vbo_indices_);

  init_framebuffer();

  gl_check_error_and_halt("init_gl_resources()");
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


/**
 * Create a framebuffer object for rendering output.
 * Delegate the configuration of this to the subclass.
 */
void RenderXform::init_framebuffer() {
  spdlog::debug("RenderXform::init_framebuffer()");
  glGenFramebuffers(1, &fbo_);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

  // Delegated to subclasses
  configure_framebuffer();

  auto err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (err != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error(fmt::format("FB not complete : {}", err));
  }

  // Unbind the FBO.
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


/**
 * Bind the FBO and Vertex Array
 */
void RenderXform::start_render() const{
  spdlog::debug("RenderXform::start_render()");

  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

  glBindVertexArray(vao_id_);
}

/*
 * Unbind them.
 */
void RenderXform::end_render(){
  spdlog::debug("RenderXform::end_render()");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}