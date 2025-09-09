//
// TNT (Triage Narrative Tool) created by 5' Digital. 
// All rights reserved.
//

#include "tools/graph_view.h"

#include <string>
#include <math.h>

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "imnodes.h"
#include "nfd.h"

#include "tools/entry_editor.h"
#include "tnt_data.h"
#include "app.h"

static int cantorPair(int x, int y) {
    int sum = x + y;
    return ((sum + 1) * sum) / 2 + y;
}

static void reverseCantorPair(int z, int& x, int& y) {
    int w = (int)floorf((sqrtf(8.f * (float)z + 1.f) - 1.f) / 2.f);
    int t = (w * w + w) / 2;
    y = z - t;
    x = w - y;
}

static int getAttributeId(int entryId, int attributeIndex) {
    // Implementation of Cantor pairing function. https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
    return cantorPair(entryId, attributeIndex + 1);
}

static int getInputAttributeId(int entryId) {
    // Implementation of Cantor pairing function. https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
    return cantorPair(entryId, 0);
}

static void getEntryAndAttributeFromId(int attributeId, int& entryId, int& attributeIndex) {
    int attributeIndexTemp;
    reverseCantorPair(attributeId, entryId, attributeIndexTemp);
    attributeIndex = attributeIndexTemp - 1;
}

void tnt::tools::GraphViewTool::doGui() {
    Tool::doGui();

    if (wantChangeDefaultSpeaker) {
        ImGui::OpenPopup("Change Default Speaker");
        wantChangeDefaultSpeaker = false;
    }
    if (ImGui::BeginPopup("Change Default Speaker")) {
        ImGui::InputText("Default Speaker", &defaultSpeaker);
        if (ImGui::Button("Save")) {
            app->saveState("Change Default Speaker");
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImNodes::BeginNodeEditor();

    for (TNTEntry& entry : app->getTntData().entries) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9, 0.9, 0.9, 1.0));

        if (entry.warpTo != UINT32_MAX) {
            ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGui::ColorConvertFloat4ToU32(ImVec4{ 0.24, 0.63, 0.29, 1.0 }));
            ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImGui::ColorConvertFloat4ToU32(ImVec4{ 0.16, 0.81, 0.25, 1.0 }));
        }
        ImNodes::BeginNode((int)entry.id);
        {
            if (freshPositions || app->isDataDirty()) {
                ImNodes::SetNodeGridSpacePos((int)entry.id, ImVec2(entry.posX, entry.posY));

                if (app->isDataDirty())
                    app->resetDataDirtyFlag();
            }

            // Title.
            ImNodes::BeginNodeTitleBar();
            if (entry.warpTo == UINT32_MAX) {
                if (entry.title.empty()) {
                    ImGui::Text("Unnamed");
                }
                else {
                    ImGui::Text("%s", entry.title.c_str());
                }
            }
            else {
                ImGui::Text(ICON_MS_TAB_MOVE " WARP to %u", entry.warpTo);
            }
            
            ImGui::SameLine();
            ImGui::TextDisabled("%u", entry.id);
            ImNodes::EndNodeTitleBar();


            // Input.
            ImNodes::BeginInputAttribute(getInputAttributeId((int)entry.id));
            ImGui::Dummy(ImVec2(1, 1));
            ImNodes::EndInputAttribute();

            // Output.
            for (int attributeIndex = 0; attributeIndex < entry.options.size(); attributeIndex++) {
                TNTOption option = entry.options[attributeIndex];
                ImNodes::BeginOutputAttribute(getAttributeId((int)entry.id, attributeIndex));
                {
                    ImGui::Text("%s", option.name.c_str());
                }
                ImNodes::EndOutputAttribute();
            }

            ImVec2 pos = ImNodes::GetNodeGridSpacePos((int)entry.id);
            entry.posX = pos.x;
            entry.posY = pos.y;


        }
        ImNodes::EndNode();
        if (entry.warpTo != UINT32_MAX) {
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
        }

        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                bool existingTool = false;
                for (Tool* tool : app->getTools()) {
                    if (tool->isEditingEntry(entry.id)) {
                        existingTool = true;
                        app->focusTool(tool);
                        break;
                    }
                }

                if (!existingTool) {
                    app->openTool(new tnt::tools::EntryEditorTool(app, entry.id));
                }
            }

        }

        for (int attributeIndex = 0; attributeIndex < entry.options.size(); attributeIndex++) {
            const TNTOption& option = entry.options[attributeIndex];
            if (option.id != UINT32_MAX) {
                ImNodes::Link(
                    cantorPair((int)entry.id, (int)option.id), getAttributeId(entry.id, attributeIndex),
                    getInputAttributeId(option.id));
            }
        }

        ImGui::PopStyleColor();
    }

    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_TopRight);
    // Check if the editor is focused.
    bool inScope = ImGui::IsWindowFocused();
    ImNodes::EndNodeEditor();

    bool deletePressed = ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace);
    if (inScope && deletePressed) {
        app->saveState("Delete Selected Entries");

        size_t selectedNodeCount = ImNodes::NumSelectedNodes();
        int* selection = new int[selectedNodeCount];
        ImNodes::GetSelectedNodes(selection);
        
        // Erase selected nodes.
        for (int selectedIdIndex = 0; selectedIdIndex < selectedNodeCount; selectedIdIndex++) {
            int selectedId = selection[selectedIdIndex];
            std::erase_if(
                app->getTntData().entries, [&](const auto& item) {
                    return item.id == selectedId;
                }
            );

            // Remove other entries' options referencing this one.
            for (auto& entry : app->getTntData().entries) {
                if (entry.id == selectedId) {
                    continue;
                }

                for (auto& option : entry.options) {
                    if (option.id == selectedId) {
                        option.id = UINT32_MAX;
                    }
                }
            }
        }
    }

    if (ImNodes::IsEditorHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        app->saveState("Add Entry");

        app->getTntData().entries.push_back(
            TNTEntry {
                .id = nextId,
                .speakerName = defaultSpeaker
            }
        );
        nextId ++;
    }

    int startAttrId, endAttrId;
    if (ImNodes::IsLinkCreated(&startAttrId, &endAttrId)) {
        app->saveState("Create Link");

        int startEntryId, startAttributeIndex;
        getEntryAndAttributeFromId(startAttrId, startEntryId, startAttributeIndex);
        int endEntryId, endAttributeIndex;
        getEntryAndAttributeFromId(endAttrId, endEntryId, endAttributeIndex);

        TNTEntry& startEntry = app->getTntData().getEntryById(startEntryId);
        TNTEntry& endEntry = app->getTntData().getEntryById(endEntryId);

        startEntry.options[startAttributeIndex].id = endEntry.id;
    }

    if (freshPositions) freshPositions = false;
}

bool tnt::tools::GraphViewTool::isBig() {
    return true;
}

std::string tnt::tools::GraphViewTool::getAboutPage() {
    return "Shows the tree of dialogue entries and allows the user to edit them.";
}

void tnt::tools::GraphViewTool::doMenuGui() {
    if (ImGui::BeginMenu("File")) {

        if (ImGui::MenuItem("Open...")) {
            char* openPath;
            int result = NFD_OpenDialog("json", app->getLastJsonPath().c_str(), &openPath);
            if (result == NFD_OKAY) {
                const char* jsonText = static_cast<const char*>(SDL_LoadFile(openPath, NULL));
                json_object* json = json_tokener_parse(jsonText);
                app->getTntData().fromJson(json);
                auto openPathStr = std::string{ openPath };
                app->setLastJsonPath(openPathStr);
                app->clearHistory();
            }
            freshPositions = true;

            uint32_t maxId = 0;
            for (TNTEntry& entry : app->getTntData().entries) {
                if (entry.id > maxId) {
                    maxId = entry.id;
                }
                nextId = maxId + 1;
            }
        }

        if (ImGui::MenuItem("Save...")) {
            json_object* json = json_object_new_array();
            app->getTntData().toJson(json);

            const char* jsonText = json_object_to_json_string(json);
            size_t jsonTextLength = SDL_strlen(jsonText);
            char* savePathRaw;
            int result = NFD_SaveDialog("json", app->getLastJsonPath().c_str(), &savePathRaw);
            if (result == NFD_OKAY) {
                std::string savePath{ savePathRaw };
                if (!savePath.ends_with(".json")) {
                    savePath += ".json";
                }

                SDL_SaveFile(savePath.c_str(), json_object_to_json_string(json), jsonTextLength);
                app->setLastJsonPath(savePath);
            }

            freshPositions = true;
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Add")) {
        if (ImGui::MenuItem("Entry")) {
            app->saveState("Add Entry");

            app->getTntData().entries.push_back(
                TNTEntry {
                    .id = nextId,
                    .speakerName = defaultSpeaker
                }
            );
            nextId++;
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Speaker")) {
        ImGui::TextDisabled("Current Default Speaker: %s", defaultSpeaker.c_str());
        if (ImGui::MenuItem("Change Default Speaker...")) {
            wantChangeDefaultSpeaker = true;
        }

        ImGui::EndMenu();
    }
}
