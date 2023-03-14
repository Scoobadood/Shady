#include "xform-io.h"
#include <gtest/gtest.h>

#include <utility>

class NoOpXform : public Xform {
public:
  NoOpXform(std::string name, XformConfig cfg) //
          : Xform(std::move(name), std::move(cfg)) //
  {}

  std::string type() const override { return "noop"; }

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string &err_msg) override {
    return {};
  }
};

class OutXform : public Xform {
public:
  OutXform(std::string name, XformConfig cfg) //
          : Xform(std::move(name), std::move(cfg)) //
  {
    add_output_port_descriptor("out_p1", "image");
    add_output_port_descriptor("out_p2", "float");
  }

  std::string type() const override { return "out_xform"; }

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string &err_msg) override {
    return {};
  }
};

class InXform : public Xform {
public:
  InXform(std::string name, XformConfig cfg) //
          : Xform(std::move(name), std::move(cfg)) //
  {
    add_input_port_descriptor("in_p1", "float");
    add_input_port_descriptor("in_p2", "image");
  }

  std::string type() const override { return "in_xform"; }

  std::map<std::string, std::shared_ptr<void>>
  do_apply(const std::map<std::string, std::shared_ptr<void>> &inputs,
           uint32_t &err, std::string &err_msg) override {
    return {};
  }
};

class TestIO : public testing::Test {
public:
};

TEST_F(TestIO, test_write_float_parm) {
  XformConfig::PropertyDescriptor pd{
          "test_float",
          XformConfig::PropertyDescriptor::FLOAT
  };

  std::ostringstream actual_str;
  xform_write_property(actual_str, 0, pd, "0.1");

  std::string expected =
          R"({
  "name" : "test_float",
  "type" : "FLOAT",
  "value" : 0.1,
},
)";
  EXPECT_EQ(expected, actual_str.str());

}

TEST_F(TestIO, test_write_int_parm) {
  XformConfig::PropertyDescriptor pd{
          "test_int",
          XformConfig::PropertyDescriptor::INT
  };

  std::ostringstream actual_str;
  xform_write_property(actual_str, 0, pd, "1");

  std::string expected =
          R"({
  "name" : "test_int",
  "type" : "INT",
  "value" : 1,
},
)";
  EXPECT_EQ(expected, actual_str.str());
}

TEST_F(TestIO, test_write_string_parm) {
  XformConfig::PropertyDescriptor pd{
          "test_string",
          XformConfig::PropertyDescriptor::STRING
  };

  std::ostringstream actual_str;
  xform_write_property(actual_str, 0, pd, "\"fish\"");

  std::string expected =
          R"({
  "name" : "test_string",
  "type" : "STRING",
  "value" : "fish",
},
)";
  EXPECT_EQ(expected, actual_str.str());
}

TEST_F(TestIO, test_write_config) {
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

  auto m = std::make_shared<NoOpXform>(
          "xform",
          XformConfig{{pd1, pd2, pd3}}
  );

  m->config().set("test_float", 0.1f);
  m->config().set("test_int", 1);
  m->config().set("test_string", "fish");

  std::ostringstream actual_str;
  xform_write(actual_str, 0, m);

  std::string expected =
          R"({
  "name" : "xform",
  "type" : "noop",
  "config" : [
    {
      "name" : "test_float",
      "type" : "FLOAT",
      "value" : 0.1,
    },
    {
      "name" : "test_int",
      "type" : "INT",
      "value" : 1,
    },
    {
      "name" : "test_string",
      "type" : "STRING",
      "value" : "fish",
    },
  ]
},
)";
  EXPECT_EQ(expected, actual_str.str());
}

TEST_F(TestIO, test_write_graph) {
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

  xf1->config().set("test_float", 0.1f);
  xf2->config().set("test_int", 1);
  xf1->config().set("test_string", "fish");

  auto g = std::make_shared<XformGraph>();
  g->add_xform(xf1);
  g->add_xform(xf2);
  g->add_connection("out_1","out_p1", "in_1", "in_p2");
  g->add_connection("out_1","out_p2", "in_1", "in_p1");

  std::ostringstream actual_str;
  xform_write_graph(actual_str, g);

  std::string expected =
          R"({
  "xforms" : [
    {
      "name" : "in_1",
      "type" : "in_xform",
      "config" : [
        {
          "name" : "test_int",
          "type" : "INT",
          "value" : 1,
        },
      ]
    },
    {
      "name" : "out_1",
      "type" : "out_xform",
      "config" : [
        {
          "name" : "test_float",
          "type" : "FLOAT",
          "value" : 0.1,
        },
        {
          "name" : "test_string",
          "type" : "STRING",
          "value" : "fish",
        },
      ]
    },
  ],
  "connections" : [
    {
      "from_xform" : "out_1",
      "from_port" : "out_p1",
      "to_xform" : "in_1",
      "to_port" : "in_p2",
    },
    {
      "from_xform" : "out_1",
      "from_port" : "out_p2",
      "to_xform" : "in_1",
      "to_port" : "in_p1",
    },
  ]
}
)";
  EXPECT_EQ(expected, actual_str.str());
}
