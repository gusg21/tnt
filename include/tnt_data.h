//
// TNT (Triage Narrative Tool) created by 5' Digital. 
// All rights reserved.
//

#pragma once

#include <vector>
#include <string>

#include "json.h"

namespace tnt {
    class TNTConditional {
    public:
        int targetValue = 0; // The value to compare against.
        std::string variable = "VariableName"; // The variable to compare.
        std::string comparison = "=="; // How to compare the two.

        void fromJson(json_object* object);
        void toJson(json_object* object) const;
    };

    class TNTOption {
    public:
        uint32_t id = UINT32_MAX; // Entry to go to.
        std::string name = "New Option"; // Option text.
        std::vector<TNTConditional> enabledCheck; // The check to see if this option is enabled.

        void fromJson(json_object* object);
        void toJson(json_object* object) const;
    };

    class TNTEvent {
    public:
        std::string name = "ExampleEvent"; // Name of the event to invoke.

        void fromJson(json_object* object);
        void toJson(json_object* object) const;
    };

    class TNTVariableEntry {
    public:
        std::string name = "VariableName"; // Name of the variable to change.
        int value = 0; // Value of the variable to set to.

        void fromJson(json_object* object);
        void toJson(json_object* object) const;
    };

    class TNTEntry {
    public:
        uint32_t id = 0; // This entry's id.
        std::string title { "" };
        std::string bodyText { "Example text!" };
        std::vector<TNTOption> options {};
        std::vector<TNTEvent> events {};
        std::vector<TNTVariableEntry> variables {};
        std::string speakerName = "Speaker 1";
        uint32_t warpTo = UINT32_MAX;
        float posX = 0;
        float posY = 0;

        void fromJson(json_object* object);
        void toJson(json_object* object) const;
    };

    class TNTData {
    public:
        std::vector<TNTEntry> entries {};

        void fromJson(json_object* object);
        void toJson(json_object* object) const;
        TNTEntry& getEntryById(uint32_t id);
    };
}