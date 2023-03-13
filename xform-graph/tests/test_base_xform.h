#ifndef IMAGE_TOYS_TEST_BASE_XFORM_H
#define IMAGE_TOYS_TEST_BASE_XFORM_H

#include "xform.h"
#include <strstream>
#include <spdlog/spdlog-inl.h>

#include <gtest/gtest.h>

class TestBaseXform : public testing::Test {
public:
  void SetUp();
  std::ostringstream oss_;

};

#endif // IMAGE_TOYS_TEST_BASE_XFORM_H
