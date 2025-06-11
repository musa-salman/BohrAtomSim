
#include "ui/components/ExpressionHelper.hpp"
#include <string>

namespace ui::components {

void ExpressionHelper::render() {
    if (ImGui::Button("?")) {
        m_isOpen = true;
    }

    if (!m_isOpen) {
        return;
    }

    if (ImGui::Begin("Expression Help", &m_isOpen,
                     ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoCollapse)) {
        ImGui::TextWrapped("You can use expressions to define potentials, "
                           "create custom plots, or filter simulation results. "
                           "Expressions are mathematical formulas that operate "
                           "over simulation data automatically.");

        ImGui::Separator();

        if (ImGui::TreeNode("1. What Can Expressions Do?")) {
            ImGui::TextWrapped("Expressions are used in three main places:");
            ImGui::BulletText("Define the atomic potential (U(r))");
            ImGui::BulletText("Create custom plots from simulation variables");
            ImGui::BulletText(
                "Filter simulation data to highlight certain regions");
            ImGui::Spacing();
            ImGui::TextWrapped("When plotting or filtering, the expression "
                               "is applied to each "
                               "simulation point automatically.");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("2. Available Variables")) {
            ImGui::TextWrapped("The following variables are always "
                               "available in expressions:");

            ImGui::BulletText("r        - radial distance from origin");
            ImGui::BulletText("r_dot    - radial velocity (dr/dt)");
            ImGui::BulletText("r_ddot   - radial acceleration (d²r/dt²)");
            ImGui::BulletText("psi      - angular position (2D)");
            ImGui::BulletText("psi_dot  - angular velocity (2D)");
            ImGui::BulletText("theta    - polar angle (3D)");
            ImGui::BulletText(
                "theta_dot, theta_ddot - angle velocity/acceleration");
            ImGui::BulletText("phi      - azimuthal angle (3D)");
            ImGui::BulletText("phi_dot  - azimuthal velocity");
            ImGui::BulletText("gamma    - Lorentz factor (1/sqrt(1 - v²/c²))");
            ImGui::BulletText(
                "delta_psi - in quantized simulations, the "
                "difference in psi between two points when r are at maximums");
            ImGui::BulletText("time     - simulation time at each point");

            ImGui::Spacing();
            ImGui::TextWrapped(
                "Only 'r' is allowed in potential expressions. All other "
                "variables can be used in filters and plots.");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("3. Constants")) {
            ImGui::TextWrapped(
                "Constants like Z (atomic number), a (screening factor), k "
                "(spring constant), or mu (decay rate) "
                "can be defined beforehand and used in expressions.");

            ImGui::TextWrapped("These are configured in the simulation setup, "
                               "not typed directly in the expression.");
            ImGui::TreePop();
        }

        ImVec4 exprColor = ImVec4(0.8f, 0.9f, 1.0f, 1.0f);
        auto renderExpr = [&](const char *label, const char *expr,
                              const char *id) {
            ImGui::BulletText("%s", label);
            ImGui::SameLine();
            ImGui::TextColored(exprColor, "%s", expr);
            ImGui::SameLine();
            if (ImGui::SmallButton(std::string("Copy##").append(id).c_str())) {
                ImGui::SetClipboardText(expr);
            }
        };
        if (ImGui::TreeNode("4. Examples")) {

            ImGui::Text("Potential Expressions (must use only 'r'):");
            renderExpr("Coulomb:", "-q^2 / r", "pot_coulomb");
            renderExpr("Screened:", "-q^2 * exp(-a * r) / r", "pot_screened");
            renderExpr("Yukawa:", "-g * exp(-mu * r) / r", "pot_yukawa");

            ImGui::Spacing();
            ImGui::Text("Plotting Expressions:");
            renderExpr("Kinetic energy:", "0.5 * r_dot^2", "plot_ke");
            renderExpr("Total energy:", "0.5 * r_dot^2 - q^2 / r", "plot_te");
            renderExpr("Angle evolution:", "psi", "plot_psi");

            ImGui::Spacing();
            ImGui::Text("Filtering Expressions:");
            renderExpr("Incoming radial:", "r > 1.0 and r_dot < 0",
                       "filt_inward");
            renderExpr("Sector + motion:",
                       "theta < pi / 2 and abs(phi_dot) > 0.01", "filt_sector");
            renderExpr("Accel check:", "r_ddot + theta_dot^2 * r < 0",
                       "filt_accel");

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("5. Advanced Potential Example")) {
            ImGui::Text("Piecewise Screened Potential:");
            ImGui::TextWrapped("This model defines different regions with "
                               "different behaviors:\n"
                               "- Softened Coulomb near origin\n"
                               "- Exponentially screened middle\n"
                               "- Pure Coulomb far away");

            const char *sw_expr =
                "switch {\n"
                "  case r < 0.2 : -q^2 / sqrt(r^2 + a^2);\n"
                "  case r < 1.0 : -q^2 * exp(-b*r)/r + 0.1*sin(5*r);\n"
                "  default      : -q^2 / r;\n}";
            ImGui::PushStyleColor(ImGuiCol_Text, exprColor);
            ImGui::TextUnformatted(sw_expr);
            ImGui::PopStyleColor();
            if (ImGui::SmallButton("Copy##adv_switch")) {
                ImGui::SetClipboardText(sw_expr);
            }

            ImGui::Spacing();
            ImGui::Text("Same logic using if/else:");
            const char *if_expr = "if (r < 0.2) {\n"
                                  "  -q^2 / sqrt(r^2 + a^2);\n"
                                  "} else if (r < 1.0) {\n"
                                  "  -q^2 * exp(-b*r)/r + 0.1*sin(5*r);\n"
                                  "} else {\n"
                                  "  -q^2 / r;\n"
                                  "}";
            ImGui::PushStyleColor(ImGuiCol_Text, exprColor);
            ImGui::TextUnformatted(if_expr);
            ImGui::PopStyleColor();
            if (ImGui::SmallButton("Copy##adv_ifelse")) {
                ImGui::SetClipboardText(if_expr);
            }

            ImGui::Spacing();
            ImGui::TextWrapped(
                "This example showcases how to use ExprTk's switch/if logic to "
                "model more realistic atomic potentials.");
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("6. Advanced Filtering Examples")) {
            ImGui::TextWrapped(
                "Expressions for filtering allow you to highlight "
                "specific regions of the simulation over time.");
            ImGui::Spacing();

            renderExpr("Turning Point Detection:",
                       "(r_dot < 0) and (r > 0.5) and (abs(psi_dot) > 0.05)",
                       "flt_turning");

            renderExpr("Bound State Filter (2D energy estimate):",
                       "(0.5 * r_dot^2 + psi_dot^2 / (2 * r^2) - Z / r) < 0",
                       "flt_bound");

            renderExpr(
                "3D Sector + Bound Energy:",
                "(theta > pi/4 and theta < 3*pi/4) and (energy_expr < 0)",
                "flt_sector_energy");

            renderExpr("Conditional (piecewise) mask:",
                       "if (r < 0.3, 1, if (r < 1.0 and r_dot < 0, 1, 0))",
                       "flt_piecewise");

            ImGui::Spacing();
            ImGui::TextWrapped(
                "These expressions return 'true' for the points to be "
                "selected (filtered). You can use them to build "
                "dynamic visual masks or extract specific orbital behaviors.");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("7. Advanced Plotting Examples")) {
            ImGui::TextWrapped(
                "Plotting expressions are evaluated at each "
                "point in the simulation. They return a numerical "
                "value which becomes a time series or curve.");

            ImGui::Spacing();

            renderExpr("Kinetic + Potential (Total Energy):",
                       "0.5 * r_dot^2 + psi_dot^2 / (2 * r^2) - q^2 / r",
                       "plot_energy");

            renderExpr("Angular Momentum Estimate (2D):", "r^2 * psi_dot",
                       "plot_ang_mom");

            renderExpr("Effective Potential (Central Force):",
                       "-q^2 / r + (psi_dot^2) / (2 * r^2)", "plot_eff_pot");

            renderExpr("Piecewise Energy Profile:",
                       "if (r < 1.0, 0.5 * r_dot^2 - q^2 / r, -q^2 / r)",
                       "plot_piecewise");

            renderExpr("Clamped Energy (avoid spikes near r ≈ 0):",
                       "0.5 * clamp(r_dot^2, 0, 10) - q^2 / max(r, 0.1)",
                       "plot_clamped");

            renderExpr("Oscillating Potential Region Marking:",
                       "sin(5 * r) * exp(-r)", "plot_osc");

            ImGui::Spacing();
            ImGui::TextWrapped("These expressions can help visualize conserved "
                               "quantities, energy variations, and transitions "
                               "between orbital states.");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("8. Supported Operators and Functions")) {
            ImGui::Text("Mathematical Operators:");
            ImGui::BulletText("+  -  *  /  %%  ^");

            ImGui::Text("Comparisons:");
            ImGui::BulletText("==  !=  <>  <  <=  >  >=");

            ImGui::Text("Logical Operators:");
            ImGui::BulletText("and, or, not, xor, nand, nor");

            ImGui::Text("Functions:");
            ImGui::BulletText("abs, sqrt, exp, log, log10, pow, mod, floor, "
                              "ceil, round, trunc, frac, clamp, sgn");

            ImGui::Text("Trigonometry:");
            ImGui::BulletText("sin, cos, tan, asin, acos, atan, atan2");
            ImGui::BulletText("sinh, cosh, tanh, asinh, acosh, atanh");

            ImGui::Text("Angle Conversion:");
            ImGui::BulletText("deg2rad, rad2deg");

            ImGui::Text("Conditionals:");
            ImGui::BulletText("if(condition, a, b)");
            ImGui::BulletText("a ? b : c");

            ImGui::Spacing();
            ImGui::TextWrapped(
                "All functions automatically apply across the simulation "
                "data. "
                "You don't need loops or arrays — it just works.");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("9. Tips")) {
            ImGui::BulletText(
                "Use parentheses to control order of operations.");
            ImGui::BulletText("Use 'pi' for π (3.14159...).");
            ImGui::BulletText(
                "Use 'abs(x)' to avoid negative values in conditions.");
            ImGui::BulletText("Plotting returns a curve; filtering returns "
                              "selected points.");
            ImGui::BulletText("Expressions are case-sensitive.");
            ImGui::BulletText(
                "Invalid expressions may return NaN or be ignored.");
            ImGui::TreePop();
        }
    }

    ImGui::End();
}
} // namespace ui::components