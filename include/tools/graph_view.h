//
// TNT (Triage Narrative Tool) created by 5' Digital. 
// All rights reserved.
//

#pragma once

#include <unordered_map>

#include "ims_icons.h"
#include "tool.h"

namespace tnt {
    namespace tools {
        class GraphViewTool : public tnt::Tool {
        public:
            explicit GraphViewTool(tnt::App* app) : tnt::Tool(app, "Graph View", ICON_MS_GRAPH_1) {}

            void doGui() override;
            void doMenuGui() override;
            bool isBig() override;
            std::string getAboutPage() override;

            bool freshPositions = true;
            uint32_t nextId = 0;
            std::string defaultSpeaker { "Default Speaker" };
            bool wantChangeDefaultSpeaker = false;
        };
    }
}