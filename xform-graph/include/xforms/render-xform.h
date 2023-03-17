#ifndef IMAGE_TOYS_RENDER_XFORM_H
#define IMAGE_TOYS_RENDER_XFORM_H

#include "xforms/xform.h"

class RenderXform : public Xform {
public:
  explicit RenderXform(const std::string &name, XformConfig config);

  ~RenderXform() override;

  /**
   * Virtual initialisation; MUST be called before object can be used.
   */
  void init() override;

protected:
  void start_render() const;

  static void end_render();

private:
  void init_framebuffer();

  /* Subclasses should implement to
   * - allocated and bind textures, depths maps etc.
   * - set up drawBuffers
   */
  virtual void configure_framebuffer() = 0;

  void init_gl_resources();

  GLuint fbo_;
  GLuint vao_id_;
  GLuint vbo_verts_;
  GLuint vbo_indices_;
};

#endif //IMAGE_TOYS_RENDER_XFORM_H
