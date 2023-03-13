#include "xform-config.h"
#include "test_config.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST_F(TestConfig, set_string_ok) {
  config_.set("a_string", "hello");

  std::string v;
  EXPECT_TRUE(config_.get("a_string", v));
  EXPECT_EQ("hello", v);
}

TEST_F(TestConfig, set_unknown_string_fails) {
  config_.set("b_string", "hello");

  std::string v;
  EXPECT_FALSE(config_.get("b_string", v));
}

TEST_F(TestConfig, set_float_ok) {
  config_.set("a_float", 3.14f);

  float f;
  EXPECT_TRUE(config_.get("a_float", f));
  EXPECT_FLOAT_EQ(3.14, f);
}

TEST_F(TestConfig, set_unknown_float_fails) {
  config_.set("b_float", 1.414f);

  float f;
  EXPECT_FALSE(config_.get("b_float", f));
}

TEST_F(TestConfig, set_int_ok) {
  config_.set("a_int", 99);

  int i;
  EXPECT_TRUE(config_.get("a_int", i));
  EXPECT_EQ(99, i);
}

TEST_F(TestConfig, set_unknown_unknown_fails) {
  config_.set("b_int", 69);

  int i;
  EXPECT_FALSE(config_.get("b_int", i));
}

TEST_F(TestConfig, get_property_descriptors) {
  auto descriptors = config_.descriptors();
  EXPECT_THAT(descriptors, testing::UnorderedElementsAre(p1, p2, p3));
}
