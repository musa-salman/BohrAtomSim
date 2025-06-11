#pragma once

#include "physics/math/macros.hpp"
#include <algorithm>
#include <cstring>
#include <gsl/zstring>
#include <imgui.h>
#include <string>

namespace ui::components {

static SIM_INLINE void
renderAutoGrowInputMultiline(gsl::czstring label, std::string &expression,
                             float minLines = 1.0f, float maxLines = 12.0f,
                             float maxWidth = 500.0f) noexcept {
    const float lineHeight = ImGui::GetTextLineHeightWithSpacing();

    const int numLines =
        std::count(expression.begin(), expression.end(), '\n') +
        (expression.empty() ? 0 : 1);

    const float visibleLines =
        std::clamp(static_cast<float>(numLines + 1), minLines, maxLines);
    const float width = std::min(ImGui::GetContentRegionAvail().x, maxWidth);
    const ImVec2 boxSize = ImVec2(width, lineHeight * visibleLines);
    {
        char buffer[2048];
        std::strncpy(buffer, expression.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::InputTextMultiline(label, buffer, sizeof(buffer), boxSize,
                                      ImGuiInputTextFlags_AllowTabInput)) {
            expression = buffer;
        }
    }
}
} // namespace ui::components
