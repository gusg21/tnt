//
// TNT (Triage Narrative Tool) created by 5' Digital. 
// All rights reserved.
//

#include "tools/entry_editor.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "ims_icons.h"

#include "tnt_data.h"
#include "app.h"

#define MIN(a, b) ((a > b) ? b : a)

void tnt::tools::EntryEditorTool::doGui() {
    TNTEntry& entry = app->getTntData().getEntryById(entryId);

    if (entry.title.empty()) {
        name = "Entry (Untitled, ID #" + std::to_string(entry.id) + ")###" + "Entry" + std::to_string(entry.id);
    } else {
        name = "Entry (" + entry.title + ")###Entry" + std::to_string(entry.id);
    }

    ImVec2 space = ImGui::GetContentRegionAvail();

    //ImGui::SetNextWindowSize(ImVec2(space.x * 0.7f, -1), ImGuiCond_Appearing);
    if (ImGui::BeginChild("##Text Area", ImVec2(space.x * 0.7f, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX,
                          ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar)) {
        bool warpEnabled = entry.warpTo != UINT32_MAX;
        if (ImGui::Checkbox("Warp", &warpEnabled)) {
            if (warpEnabled) {
                entry.warpTo = 0;
            }
            else {
                entry.warpTo = UINT32_MAX;
            }
        }        
        if (warpEnabled) {
            ImGui::SameLine();
            ImGui::InputInt("##warp id", (int*)&entry.warpTo);

        }

        ImGui::BeginDisabled(warpEnabled);
        ImVec2 space2 = ImGui::GetContentRegionAvail();
        ImGui::SeparatorText("Text");
        ImGui::InputTextMultiline("##Input", &entry.bodyText, ImVec2(-1, space2.y * 0.8));
        if (ImGui::CollapsingHeader("Info", ImGuiTreeNodeFlags_DefaultOpen)){
            if (ImGui::BeginChild("Info Group", ImVec2(-1, space2.y * 0.2))) {
                ImGui::InputText("Title", &entry.title);
                ImGui::InputText("Speaker", &entry.speakerName);
                int newId = entry.id;
                int oldId = entry.id;
                ImGui::InputInt("ID", reinterpret_cast<int*>(&newId));
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    for (TNTEntry& otherEntry : app->getTntData().entries) {
                        if (otherEntry.id == newId) {
                            otherEntry.id = oldId;
                            entry.id = newId;
                            break;
                        }
                    }
                    for (TNTEntry& otherEntry : app->getTntData().entries) {
                        for (TNTOption& otherEntryOption : otherEntry.options) {
                            if (otherEntryOption.id == newId) {
                                otherEntryOption.id = oldId;
                            }
                            else if (otherEntryOption.id == oldId) {
                                otherEntryOption.id = newId;
                            }
                        }
                    }
                }
            }
            ImGui::EndChild();
        }
//        this->editor.Render("Test", ImVec2(-1, -1), true);

        ImGui::EndDisabled();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Tabs
    if (ImGui::BeginChild("##Tabs", ImVec2(-1, 0), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX, 0)) {
        ImGui::BeginTabBar("##Editor Tab Bar");

        // Options
        if (ImGui::BeginTabItem(ICON_MS_SETTINGS "Options")) {
            ImGui::SeparatorText("Options");
            if (ImGui::Button(ICON_MS_ADD " Add")) {
                app->saveState("Add Option");
                entry.options.push_back(TNTOption { });
            }
            uint32_t index = 0;
            uint32_t removeIndex = UINT32_MAX;
            for (TNTOption& option : entry.options) {
                if (ImGui::BeginChild(("##" + std::to_string(index)).c_str(), ImVec2(0, 140),
                                      ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY
                )) {
                    // Option name.
                    ImGui::InputText("Name", &option.name);

                    // Show conditionals.
                    if (ImGui::CollapsingHeader("Conditionals")) {
                        if (ImGui::Button(ICON_MS_ADD " Add Conditional")) {
                            app->saveState("Add Conditional");
                            option.enabledCheck.push_back(TNTConditional { });
                        }
                        uint32_t indexCond = 0;
                        uint32_t removeIndexCond = UINT32_MAX;
                        for (TNTConditional& conditional : option.enabledCheck) {
                            if (ImGui::BeginChild(("##" + std::to_string(indexCond + 500)).c_str(), ImVec2(0, 140),
                                                  ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY
                            )) {
                                ImGui::InputText("Variable", &conditional.variable);
                                ImGui::InputInt("Target Value", &conditional.targetValue);
                                ImGui::InputText("Comparison", &conditional.comparison);
                                if (ImGui::Button("Delete")) {
                                    app->saveState("Delete Conditional");
                                    removeIndexCond = index;
                                }
                                ImGui::SameLine();
                            }
                            ImGui::EndChild();
                            indexCond++;
                        }
                        if (removeIndexCond != UINT32_MAX) {
                            option.enabledCheck.erase(option.enabledCheck.begin() + removeIndexCond);
                        }
                    }

                    // Deleting options.
                    if (ImGui::Button("Delete")) {
                        app->saveState("Delete Option");
                        removeIndex = index;
                    }
                    ImGui::SameLine();
                    if (option.id == UINT32_MAX) {
                        ImGui::TextColored(ImVec4(0.8, 0.8, 0.2, 1.0), "Not Connected!");
                    } else {
                        ImGui::TextDisabled("Connected to ID #%d", option.id);
                    }
                }
                ImGui::EndChild();
                index++;
            }
            if (removeIndex != UINT32_MAX) {
                entry.options.erase(entry.options.begin() + removeIndex);
            }

            if (entry.options.empty()) {
                ImGui::TextDisabled("Is Leaf Node? Yes");
            } else {
                ImGui::TextDisabled("Is Leaf Node? No");
            }

            ImGui::EndTabItem();
        }

        // Events
        if (ImGui::BeginTabItem(ICON_MS_FLAG "Events")) {
            ImGui::SeparatorText("Events");

            if (ImGui::Button("+ Add")) {
                app->saveState("Add Event");
                entry.events.push_back(TNTEvent { });
            }
            uint32_t index = 0;
            uint32_t removeIndex = UINT32_MAX;
            for (TNTEvent& event : entry.events) {
                if (ImGui::BeginChild(("##" + std::to_string(index)).c_str(), ImVec2(0, 60),
                                      ImGuiChildFlags_Border
                )) {
                    ImGui::InputText("Event Name", &event.name);
                    if (ImGui::Button("Delete")) {
                        app->saveState("Delete Event");
                        removeIndex = index;
                    }
                }
                ImGui::EndChild();
                index++;
            }
            if (removeIndex != UINT32_MAX) {
                entry.events.erase(entry.events.begin() + removeIndex);
            }

            ImGui::EndTabItem();
        }


        // Variables
        if (ImGui::BeginTabItem(ICON_MS_PEN_SIZE_5 "Variables")) {
            ImGui::SeparatorText("Variables");
            if (ImGui::Button("+ Add")) {
                app->saveState("Add Variable");
                entry.variables.push_back(TNTVariableEntry{ });
            }
            uint32_t index = 0;
            uint32_t removeIndex = UINT32_MAX;
            for (TNTVariableEntry& variable : entry.variables) {
                if (ImGui::BeginChild(("##" + std::to_string(index)).c_str(), ImVec2(0, 100),
                    ImGuiChildFlags_Border
                )) {
                    ImGui::InputText("Name", &variable.name);
                    ImGui::InputInt("Value", &variable.value);
                    if (ImGui::Button("Delete")) {
                        app->saveState("Delete Variable");
                        removeIndex = index;
                    }
                }
                ImGui::EndChild();
                index++;
            }
            if (removeIndex != UINT32_MAX) {
                entry.variables.erase(entry.variables.begin() + removeIndex);
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::EndChild();

}

void tnt::tools::EntryEditorTool::doMenuGui()
{
    if (ImGui::BeginMenu("Tool")) {
        if (ImGui::MenuItem("Close")) {
            app->closeTool(this);
        }
        ImGui::EndMenu();
    }
}

bool tnt::tools::EntryEditorTool::isEditingEntry(uint32_t id) {
    return id == entryId;
}

std::string tnt::tools::EntryEditorTool::getAboutPage() {
    return "Edits a single dialogue node.";
}

tnt::tools::EntryEditorTool::EntryEditorTool(tnt::App* app, uint32_t entryId)
    : tnt::Tool(app, "Entry Editor", ICON_MS_SCRIPT) {
    this->entryId = entryId;
    this->editor = TextEditor {};
    this->editor.SetReadOnly(false);
}
