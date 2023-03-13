#ifndef IMAGE_TOYS_XFORM_TEXTURE_META_H
#define IMAGE_TOYS_XFORM_TEXTURE_META_H

#include <OpenGL/gl3.h>

struct TextureMetadata {
  const GLuint texture_id;
  const GLsizei width;
  const GLsizei height;
  TextureMetadata(const GLuint texture_id,
                  const GLsizei width,
                  const GLsizei height)//
          : texture_id{texture_id} //
          , width{width} //
          , height{height} //
  {}
};

#endif //IMAGE_TOYS_XFORM_TEXTURE_META_H
