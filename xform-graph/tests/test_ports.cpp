#include "xform-port.h"
#include <gtest/gtest.h>

TEST(TestPorts, default_compatibility) {
  InputPortDescriptor ipd{"ipd1", "image"};
  OutputPortDescriptor opd{"opd1", "image"};
  EXPECT_TRUE(ipd.is_compatible(opd));
}

TEST(TestPorts, default_incompatibility) {
  InputPortDescriptor ipd{"ipd1", "image"};
  OutputPortDescriptor opd{"opd1", "mirage"};
  EXPECT_FALSE(ipd.is_compatible(opd));
}
