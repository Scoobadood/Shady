//
// Created by Dave Durbin on 13/3/2023.
//

#ifndef IMAGE_TOYS_TEST_CONFIG_H
#define IMAGE_TOYS_TEST_CONFIG_H

#include <gtest/gtest.h>
#include "xform-config.h"

class TestConfig : public testing::Test {
public:

  XformConfig::PropertyDescriptor p1{"a_string", XformConfig::PropertyDescriptor::STRING};
  XformConfig::PropertyDescriptor p2{"a_float", XformConfig::PropertyDescriptor::FLOAT};
  XformConfig::PropertyDescriptor p3{"a_int", XformConfig::PropertyDescriptor::INT};
  XformConfig config_{{p1, p2, p3}};
};


#endif //IMAGE_TOYS_TEST_CONFIG_H
