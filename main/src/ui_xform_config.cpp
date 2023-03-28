
#include "xforms/xform-config.h"
#include "ui_state.h"

void maybe_render_config(const std::shared_ptr<const Xform> &xform,
                         State &state) {
  if (xform->config().descriptors().empty()) return;

  if (ImGui::CollapsingHeader("Config", ImGuiTreeNodeFlags_None)) {
    for (const auto &pd: xform->config().descriptors()) {
      switch (pd.type) {
        case XformConfig::PropertyDescriptor::FLOAT: {
          float f;
          ImGui::InputFloat(pd.name.c_str(), &f, 0.01f, 0.1f, "%0.3f");
          XformConfig c = xform->config();
          c.set(pd.name, f);
        }
          break;

        case XformConfig::PropertyDescriptor::INT: {
          int i;
          xform->config().get(pd.name, i);

          if( ImGui::InputInt(pd.name.c_str(), &i, 1, 10,  ImGuiInputTextFlags_EnterReturnsTrue ) ) {
            const Xform * xfc = xform.get();
            auto *xf = const_cast<Xform *>(xfc);
            xf->config().set(pd.name, i);
            state.graph->evaluate();
          }
        }
          break;


        case XformConfig::PropertyDescriptor::STRING: {
          char buf[40];
          ImGui::InputText(pd.name.c_str(), buf, 39);
          XformConfig c = xform->config();
          c.set(pd.name, std::string(buf));
        }
          break;
        default:
          ImGui::Text("%s", pd.name.c_str());
      }
    }
  }
}