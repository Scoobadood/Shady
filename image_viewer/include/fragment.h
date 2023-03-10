#ifndef IMAGE_TOYS_FRAGMENT_H
#define IMAGE_TOYS_FRAGMENT_H

/* Fragment shader for rendering an image */

#include <OpenGL/gl3.h>

const GLchar *fragment_shader_source[] = {R"(

#version 410 core

layout (location=0) out vec4 vFragColor;

smooth in vec2 vUV;

uniform sampler2D textureMap;

void main() {
  vFragColor = texture(textureMap, vUV);
}

)"};


#endif //IMAGE_TOYS_FRAGMENT_H
