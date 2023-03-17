#include "xform-io.h"
#include "xform-factory.h"
#include "xform-config.h"

#include <gtest/gtest.h>
#include <utility>

class NoOpXform : public Xform {
public:
  NoOpXform(std::string name, XformConfig cfg) //
          : Xform(std::move(name), std::move(cfg)) //
  {}

  std::string type() const override { return "noop"; }

  void init() override {};

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string &err_msg) override {
    return {};
  }
};

class OutXform : public Xform {
public:
  explicit OutXform(std::string name, XformConfig cfg = XformConfig{{}}) //
          : Xform(std::move(name), std::move(cfg)) //
  {
    add_output_port_descriptor("out_p1", "image");
    add_output_port_descriptor("out_p2", "float");
  }

  std::string type() const override { return "OutXform"; }

  void init() override {};

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string &err_msg) override {
    return {};
  }
};

class InXform : public Xform {
public:
  explicit InXform(std::string name, XformConfig cfg = XformConfig{{}}) //
          : Xform(std::move(name), cfg) //
  {
    add_input_port_descriptor("in_p1", "float");
    add_input_port_descriptor("in_p2", "image");
  }

  void init() override {};


  std::string type() const override { return "InXform"; }

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string &err_msg) override {
    return {};
  }
};

REGISTER_CLASS(InXform);

REGISTER_CLASS(OutXform);


class TestIO : public testing::Test {
public:
};

std::shared_ptr<XformGraph>
make_graph() {
  XformConfig::PropertyDescriptor pd1{
          "test_float",
          XformConfig::PropertyDescriptor::FLOAT
  };
  XformConfig::PropertyDescriptor pd2{
          "test_string",
          XformConfig::PropertyDescriptor::STRING
  };
  XformConfig::PropertyDescriptor pd3{
          "test_int",
          XformConfig::PropertyDescriptor::INT
  };

  auto xf1 = std::make_shared<OutXform>(
          "out_1",
          XformConfig{{pd1, pd2}}
  );
  auto xf2 = std::make_shared<InXform>(
          "in_1",
          XformConfig{{pd3}}
  );

  xf1->config().set("test_float", 0.5f);
  xf2->config().set("test_int", 1);
  xf1->config().set("test_string", "fish");

  auto g = std::make_shared<XformGraph>();
  g->add_xform(xf1);
  g->add_xform(xf2);
  g->add_connection("out_1", "out_p1", "in_1", "in_p2");
  g->add_connection("out_1", "out_p2", "in_1", "in_p1");
  return g;
}

TEST_F(TestIO, test_write_graph) {

  auto graph = make_graph();

  std::ostringstream actual_str;
  xform_write_graph(actual_str, graph);

  std::string expected =
          R"({
  "xforms": [
    {
      "name": "in_1",
      "type": "InXform",
      "config": [
        {
          "name": "test_int",
          "type": "INT",
          "value": 1
        }
      ]
    },
    {
      "name": "out_1",
      "type": "OutXform",
      "config": [
        {
          "name": "test_float",
          "type": "FLOAT",
          "value": 0.5
        },
        {
          "name": "test_string",
          "type": "STRING",
          "value": "fish"
        }
      ]
    }
  ],
  "connections": [
    {
      "from_xform": "out_1",
      "from_port": "out_p1",
      "to_xform": "in_1",
      "to_port": "in_p2"
    },
    {
      "from_xform": "out_1",
      "from_port": "out_p2",
      "to_xform": "in_1",
      "to_port": "in_p1"
    }
  ]
}
)";
  EXPECT_EQ(expected, actual_str.str());
}

TEST_F(TestIO, round_trip_graph) {
  auto g = make_graph();
  std::ostringstream os;
  xform_write_graph(os, g);
  auto str = os.str();

  std::istringstream is{str};
  auto g2 = xform_read_graph(is);

  EXPECT_EQ(g2->xforms().size(), g->xforms().size());

}