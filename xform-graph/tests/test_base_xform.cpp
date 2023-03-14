#include "xform-port.h"
#include "xform.h"
#include "test_base_xform.h"


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/sinks/ostream_sink.h>

void TestBaseXform::SetUp() {
  auto ostream_logger = spdlog::get("gtest_logger");
  if (!ostream_logger) {
    auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_st>(oss_);
    ostream_logger = std::make_shared<spdlog::logger>("gtest_logger", ostream_sink);
    ostream_logger->set_pattern(">%v<");
    ostream_logger->set_level(spdlog::level::debug);
  }
  spdlog::set_default_logger(ostream_logger);
}


TEST_F(TestBaseXform, default_to_no_inputs) {
  class NoOpXform : public Xform {
  public:
    NoOpXform() : Xform("Noop") {}
    std::string type() const override { return "noop";}

    std::map<std::string, std::shared_ptr<void>>
    do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
             uint32_t &err, std::string &err_msg) override {
      return {};
    }
  };
  auto n = new NoOpXform();
  EXPECT_EQ(0, n->input_port_descriptors().size());
}

TEST_F(TestBaseXform, adding_two_inputs_should_fail) {
  class TXform : public Xform {
  public:
    TXform() : Xform("Noop") {
      add_input_port_descriptor("fish", "image");
      add_input_port_descriptor("fish", "image");
    }
    std::string type() const override { return "noop";}

    std::map<std::string, std::shared_ptr<void>>
    do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
             uint32_t &err, std::string &err_msg) override {
      return {};
    }
  };

  auto tx = new TXform();
  std::string test = oss_.str();
  EXPECT_EQ(1, tx->input_port_descriptors().size());
  EXPECT_TRUE(test.find("Xform Noop replacing existing input port descriptor with name fish") != std::string::npos);
  oss_.str("");
}

TEST_F(TestBaseXform, adding_two_different_inputs_should_not_fail) {
  class TXform : public Xform {
  public:
    TXform() : Xform("Noop") {
      add_input_port_descriptor("fish", "image");
      add_input_port_descriptor("fish2", "image");
    }
    std::string type() const override { return "noop";}

    std::map<std::string, std::shared_ptr<void>>
    do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
             uint32_t &err, std::string &err_msg) override {
      return {};
    }
  };

  auto tx = new TXform();
  std::string test = oss_.str();
  EXPECT_EQ(2, tx->input_port_descriptors().size());
  std::vector<std::string> names;
  for (auto &ipd: tx->input_port_descriptors()) {
    names.push_back(ipd->name());
  }
  EXPECT_THAT(names, testing::UnorderedElementsAre("fish", "fish2"));
  oss_.str("");
}
