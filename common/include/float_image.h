//
// Created by Dave Durbin on 9/3/2023.
//

#ifndef IMAGE_TOYS_COMMON_FLOAT_IMAGE_H
#define IMAGE_TOYS_COMMON_FLOAT_IMAGE_H

#include <cstdint>
#include <memory>

class FloatImage {
public:
  FloatImage(uint32_t width, uint32_t height);

private:
  uint32_t width_;
  uint32_t height_;
  std::unique_ptr<float[]> data_;
};

#endif // IMAGE_TOYS_COMMON_FLOAT_IMAGE_H
