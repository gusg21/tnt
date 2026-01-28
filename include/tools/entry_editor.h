//
// TNT (Triage Narrative Tool) created by 5' Digital. 
// All rights reserved.
//

#pragma once

#include "tool.h"

#include "TextEditor.h"

namespace tnt {
    namespace tools {
    class EntryEditorTool : public tnt::Tool {
    public:
        EntryEditorTool(tnt::App* app, uint32_t entryId);

        void doGui() override;
        void doMenuGui() override;
        bool isEditingEntry(uint32_t id) override;
        std::string getAboutPage() override;

        uint32_t entryId;
        TextEditor editor;
    };
    }
}
