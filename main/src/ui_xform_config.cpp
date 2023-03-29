
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
          xform->config().get(pd.name, f);

          if (ImGui::InputFloat(pd.name.c_str(), &f, .1, 10., "%0.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
            const Xform *xfc = xform.get();
            auto *xf = const_cast<Xform *>(xfc);
            xf->config().set(pd.name, f);
            state.graph->evaluate();
          }
        }
          break;

        case XformConfig::PropertyDescriptor::INT: {
          int i;
          xform->config().get(pd.name, i);

          if (ImGui::InputInt(pd.name.c_str(), &i, 1, 10, ImGuiInputTextFlags_EnterReturnsTrue)) {
            const Xform *xfc = xform.get();
            auto *xf = const_cast<Xform *>(xfc);
            xf->config().set(pd.name, i);
            state.graph->evaluate();
          }
        }
          break;


        case XformConfig::PropertyDescriptor::STRING: {
          std::string s;
          char buff[40];
          memset(buff, 0, 40);
          strncpy(buff, s.c_str(), 39);
          xform->config().get(pd.name, s);
          if (ImGui::InputText(pd.name.c_str(), buff, 40, ImGuiInputTextFlags_EnterReturnsTrue)) {
            const Xform *xfc = xform.get();
            auto *xf = const_cast<Xform *>(xfc);
            xf->config().set(pd.name, std::string{buff});
            state.graph->evaluate();
          }
        }
          break;
      }
    }
  }
}