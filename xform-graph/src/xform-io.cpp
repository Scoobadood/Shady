#include "xform-io.h"
#include "xform-graph.h"
#include "xforms/xform-config.h"
#include "xform-factory.h"

#include <fstream>
#include <iomanip>

#include "nlohmann/json.hpp"

#include <spdlog/spdlog-inl.h>

XformConfig::PropertyDescriptor::Type type_for_string(const std::string &s) {
  std::string str = s;
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
  if ("FLOAT" == s) return XformConfig::PropertyDescriptor::FLOAT;
  if ("INT" == s) return XformConfig::PropertyDescriptor::INT;
  if ("STRING" == s) return XformConfig::PropertyDescriptor::STRING;
  spdlog::error("Invalid tpye {}", s);
  return XformConfig::PropertyDescriptor::STRING;
}

int32_t
xform_write_graph(std::ostream &os, const std::shared_ptr<XformGraph> &graph) {
  using namespace std;
  using json = nlohmann::ordered_json;

  json g_j;
  for (const auto &x: graph->xforms()) {
    json x_j;
    x_j["name"] = x->name();
    x_j["type"] = x->type();

    if (!x->config().descriptors().empty()) {
      json cfg_j;
      for (const auto &pd: x->config().descriptors()) {
        json p_j;
        p_j["name"] = pd.name;
        p_j["type"] = pd.type_name();
        if (pd.type == XformConfig::PropertyDescriptor::STRING) {
          std::string value;
          x->config().get(pd.name, value);
          p_j["value"] = value;
        } else if (pd.type == XformConfig::PropertyDescriptor::FLOAT) {
          float value;
          x->config().get(pd.name, value);
          p_j["value"] = value;
        } else /* pd.type == INT */ {
          int value;
          x->config().get(pd.name, value);
          p_j["value"] = value;
        }
        cfg_j.emplace_back(p_j);
      }
      x_j["config"] = cfg_j;
    }
    g_j["xforms"].emplace_back(x_j);
  }

  for (const auto &conn: graph->connections()) {
    json c_j;
    c_j["from_xform"] = conn.first.first;
    c_j["from_port"] = conn.first.second;
    c_j["to_xform"] = conn.second.first;
    c_j["to_port"] = conn.second.second;
    g_j["connections"].emplace_back(c_j);
  }

  os << setw(2) << g_j << endl;

  return 0;
}


int32_t
save_graph(const std::string &file_name, const std::shared_ptr<XformGraph> &graph) {
  using namespace std;

  ofstream of{file_name};
  if (!of.is_open()) {
    spdlog::error("Couldn't open file {} to save graph", file_name);
    return 1;
  }
  xform_write_graph(of, graph);
  of.close();
  return 0;
}

std::shared_ptr<XformGraph> xform_read_graph(std::istream &is) {
  using json = nlohmann::json;
  json graph_json = json::parse(is);

  auto graph = std::make_shared<XformGraph>();

  auto xforms = graph_json.at("xforms");
  for (auto &xf_j: xforms) {

    std::shared_ptr<Xform> xform = nullptr;

    std::string name = xf_j.at("name");
    std::string type = xf_j.at("type");


    /* Config may be present, in which case parse it, otherwise we
     * assume an empty config
     */
    if (xf_j.contains("config")) {
      auto cfg = xf_j.at("config");

      std::vector<const XformConfig::PropertyDescriptor> pds;
      for (auto &cf_j: cfg) {
        std::string prop_name = cf_j.at("name");
        auto prop_type = type_for_string(cf_j.at("type"));
        pds.push_back({prop_name, prop_type});
      }
      XformConfig conf{pds};
      std::map<std::string, std::string> config;

      for (auto &cf_j: cfg) {
        auto prop_name = cf_j.at("name");
        std::string prop_value;
        if (cf_j.at("value").is_number()) {
          prop_value = to_string(cf_j.at("value"));
        } else {
          prop_value = cf_j.at("value");
        }
        config.emplace(prop_name, prop_value);
      }
      xform = XformFactory::make_xform(type, name, config);
    } else {
      // No config
      xform = XformFactory::make_xform(type, name, {});
    }
    graph->add_xform(xform);
  }

  if (graph_json.contains("connections")) {
    auto connections = graph_json.at("connections");
    for (auto con_j: connections) {
      std::string fx = con_j.at("from_xform");
      std::string fp = con_j.at("from_port");
      std::string tx = con_j.at("to_xform");
      std::string tp = con_j.at("to_port");
      if (!graph->add_connection(fx, fp, tx, tp)) {
        spdlog::error("load_graph() couldn't add connection from {}::{} to {}::{}", fx, fp, tx, tp);
        return nullptr;
      }
    }
  }
  return graph;
}

std::shared_ptr<XformGraph>
load_graph(const std::string &file_name) {

  std::ifstream f(file_name);
  if (f.fail()) {
    spdlog::error("File not found {}", file_name);
    return nullptr;
  }

  return xform_read_graph(f);
}
