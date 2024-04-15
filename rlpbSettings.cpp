#include "pch.h"
#include "rlpb.h"

void rlpb::RenderSettings() {
    ImGui::TextUnformatted("A plugin to let your Twitch chat know what preset you're rocking :)");

    if (ImGui::Button("Ball On Top")) {
        gameWrapper->Execute([this](GameWrapper* gw) {
            cvarManager->executeCommand("BetterBallOnTop");
            });
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Activate Ball On Top");
    }

    if (ImGui::Button("Something about le car")) {
        gameWrapper->Execute([this](GameWrapper* gw) {
            cvarManager->executeCommand("spitPreset");
            });
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("pres");
    }

    CVarWrapper enableCvar = cvarManager->getCvar("rlpb_enabled");
    if (!enableCvar) { return; }
    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable Plugin", &enabled)) {
        enableCvar.setValue(enabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Toggle Better Plugin");
    }

    CVarWrapper distanceCvar = cvarManager->getCvar("rlpb_distance");
    if (!distanceCvar) { return; }
    float distance = distanceCvar.getFloatValue();

    if (ImGui::SliderFloat("Distance", &distance, 0.0, 500.0)) {
        distanceCvar.setValue(distance);
    }
    if (ImGui::IsItemHovered()) {
        std::string hoverText = "Distance is " + std::to_string(distance);
        ImGui::SetTooltip(hoverText.c_str());
    }
}

