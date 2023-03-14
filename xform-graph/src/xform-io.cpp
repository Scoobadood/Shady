#include "xform-io.h"
#include "xform-graph.h"

#include <fstream>
#include <iomanip>
#include <spdlog/spdlog-inl.h>


std::string string_value_of_pd(const XformConfig::PropertyDescriptor &pd, const XformConfig &c) {
  std::string value;
  switch (pd.type) {
    case XformConfig::PropertyDescriptor::STRING:
      c.get(pd.name, value);
      value = fmt::format("\"{}\"", value);
      break;
    case XformConfig::PropertyDescriptor::FLOAT: {
      float f;
      c.get(pd.name, f);
      value = fmt::format("{}", f);
      break;
    }
    case XformConfig::PropertyDescriptor::INT: {
      int i;
      c.get(pd.name, i);
      value = fmt::format("{}", i);
      break;
    }
    default:
      spdlog::warn("Unrecognised parameter type {}", pd.type);
      value = "";
      break;
  }
  return value;
}


/*
 * Properties are written:
 * type may be FLOAT, INT or STRING.
 * Case is ignored.

     {
        "name" : "p1",
        "type" : "float",
        "value" : 0.1,
     },

 */
void xform_write_property(std::ostream &os,
                          int indent,
                          const XformConfig::PropertyDescriptor &pd,
                          const std::string &value) {
  using namespace std;

  static const std::string pd_type_name[]{
          "STRING", "FLOAT", "INT"
  };

  os << setw(indent + 1) << "{" << endl;
  os << setw(indent + 8) << "\"name\"" << " : \"" << pd.name << "\"," << endl;
  os << setw(indent + 8) << "\"type\"" << " : \"" << pd_type_name[pd.type] << "\"," << endl;
  os << setw(indent + 9) << "\"value\"" << " : " << value << "," << endl;
  os << setw(indent + 2) << "}," << endl;
}

/*
 {
   "name": "xform_name",
   "type": "xform_type",
   "config" : [
      property1,
      property2,
      ...
   ],
}

 */
void xform_write(std::ostream &os,
                 int indent,
                 const std::shared_ptr<const Xform> &xform) {
  using namespace std;

  os << setw(indent + 1) << "{" << endl;
  os << setw(indent + 8) << "\"name\"" << " : \"" << xform->name() << "\"," << endl;
  os << setw(indent + 8) << "\"type\"" << " : \"" << xform->type() << "\"," << endl;
  os << setw(indent + 10) << "\"config\"" << " : [" << endl;
  const auto &c = xform->config();
  for (const auto &pd: xform->config().descriptors()) {
    auto value = string_value_of_pd(pd, c);
    xform_write_property(os, indent + 4, pd, value);
  }
  os << setw(indent + 3) << "]" << endl;
  os << setw(indent + 2) << "}," << endl;
}

void
xform_write_connection(std::ostream &os,
                       int indent,
                       const std::pair<std::pair<std::string, std::string>,
                               std::pair<std::string, std::string>> &conn) {
  using namespace std;
  os << setw(indent + 1) << "{" << endl;
  os << setw(indent + 14) << "\"from_xform\"" << " : \"" << conn.first.first << "\"," << endl;
  os << setw(indent + 13) << "\"from_port\"" << " : \"" << conn.first.second << "\"," << endl;
  os << setw(indent + 12) << "\"to_xform\"" << " : \"" << conn.second.first << "\"," << endl;
  os << setw(indent + 11) << "\"to_port\"" << " : \"" << conn.second.second << "\"," << endl;
  os << setw(indent + 2) << "}," << endl;
}


int32_t
xform_write_graph(std::ostream &os, const std::shared_ptr<XformGraph> &graph) {
  using namespace std;

  os << "{" << endl;
  os << setw(10) << "\"xforms\"" << " : [" << endl;
  for (const auto &x: graph->xforms()) {
    xform_write(os, 4, x);
  }
  os << setw(4) << "]," << endl;
  os << setw(15) << "\"connections\"" << " : [" << endl;
  for (const auto &conn: graph->connections()) {
    xform_write_connection(os, 4, conn);
  }
  os << setw(3) << "]" << endl;
  os << "}" << endl;

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

std::shared_ptr<XformGraph>
load_graph(const std::string &file_name) {
  return nullptr;
}
