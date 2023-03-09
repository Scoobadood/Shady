#include "float_image.h"

#include <cstdint>

FloatImage::FloatImage(uint32_t width, uint32_t height){
  height_ = height;
  width_ = width;
  data_ = std::unique_ptr<float[]> (new float[width * height]);
}
