//
// TNT (Triage Narrative Tool) created by 5' Digital. 
// All rights reserved.
//

#include "tnt_data.h"

tnt::TNTEntry& tnt::TNTData::getEntryById(uint32_t id) {
    for (TNTEntry& entry : entries) {
        if (entry.id == id) {
            return entry;
        }
    }

    return entries[0];
}

void tnt::TNTData::toJson(json_object* object) const {
    for (TNTEntry entry : entries) {
        json_object* entryJson = json_object_new_object();
        entry.toJson(entryJson);
        json_object_array_add(object, entryJson);
    }
}

void tnt::TNTData::fromJson(json_object* object) {
    {
        entries.clear();
        size_t entriesArrayLength = json_object_array_length(object);
        for (int i = 0; i < entriesArrayLength; i++) {
            json_object* entryJson = json_object_array_get_idx(object, i);
            TNTEntry entry;
            entry.fromJson(entryJson);
            entries.push_back(entry);
        }
    }
}

void tnt::TNTEntry::toJson(json_object* object) const {
    json_object_object_add(object, "Name", json_object_new_string(("Entry" + std::to_string(id)).c_str()));
    json_object_object_add(object, "ID", json_object_new_int((int)id));
    json_object_object_add(object, "Title", json_object_new_string(title.c_str()));
    json_object_object_add(object, "BodyText", json_object_new_string(bodyText.c_str()));
    json_object_object_add(object, "SpeakerName", json_object_new_string(speakerName.c_str()));
    json_object_object_add(object, "PosX", json_object_new_double(posX));
    json_object_object_add(object, "PosY", json_object_new_double(posY));
    json_object_object_add(object, "WarpTo", json_object_new_int(warpTo));
    json_object_object_add(object, "IsLeaf", json_object_new_boolean(options.empty()));

    json_object* optionsArray = json_object_new_array();
    for (const TNTOption& option : options) {
        json_object* optionJson = json_object_new_object();
        option.toJson(optionJson);
        json_object_array_add(optionsArray, optionJson);
    }
    json_object_object_add(object, "Options", optionsArray);

    json_object* eventsArray = json_object_new_array();
    for (const TNTEvent& event : events) {
        json_object* eventJson = json_object_new_object();
        event.toJson(eventJson);
        json_object_array_add(eventsArray, eventJson);
    }
    json_object_object_add(object, "Events", eventsArray);

    json_object* variablesArray = json_object_new_array();
    for (const TNTVariableEntry& variableEntry : variables) {
        json_object* variableJson = json_object_new_object();
        variableEntry.toJson(variableJson);
        json_object_array_add(variablesArray, variableJson);
    }
    json_object_object_add(object, "Variables", variablesArray);
}

void tnt::TNTEntry::fromJson(json_object* object) {
    title = json_object_get_string(json_object_object_get(object, "Title"));
    speakerName = json_object_get_string(json_object_object_get(object, "SpeakerName"));
    bodyText = json_object_get_string(json_object_object_get(object, "BodyText"));
    id = json_object_get_int(json_object_object_get(object, "ID"));
    posX = (float)json_object_get_double(json_object_object_get(object, "PosX"));
    posY = (float)json_object_get_double(json_object_object_get(object, "PosY"));
    json_object* warpToJson = json_object_object_get(object, "WarpTo");
    if (warpToJson) {
        warpTo = json_object_get_int(warpToJson);
    }
    else {
        warpTo = UINT32_MAX;
    }

    {
        options.clear();
        json_object* optionsArray = json_object_object_get(object, "Options");
        size_t optionsArrayLength = json_object_array_length(optionsArray);
        for (int i = 0; i < optionsArrayLength; i++) {
            json_object* optionJson = json_object_array_get_idx(optionsArray, i);
            TNTOption option;
            option.fromJson(optionJson);
            options.push_back(option);
        }
    }

    {
        events.clear();
        json_object* eventsArray = json_object_object_get(object, "Events");
        size_t eventsArrayLength = json_object_array_length(eventsArray);
        for (int i = 0; i < eventsArrayLength; i++) {
            json_object* eventJson = json_object_array_get_idx(eventsArray, i);
            TNTEvent event;
            event.fromJson(eventJson);
            events.push_back(event);
        }
    }

    {
        variables.clear();
        json_object* variablesArray = json_object_object_get(object, "Variables");
        size_t variablesArrayLength = json_object_array_length(variablesArray);
        for (int i = 0; i < variablesArrayLength; i++) {
            json_object* variableJson = json_object_array_get_idx(variablesArray, i);
            TNTVariableEntry variable;
            variable.fromJson(variableJson);
            variables.push_back(variable);
        }
    }
}

void tnt::TNTOption::toJson(json_object* object) const {
    json_object_object_add(object, "ID", json_object_new_int((int)id));
    json_object_object_add(object, "Name", json_object_new_string(name.c_str()));
    json_object* conditionsArray = json_object_new_array();
    json_object_object_add(object, "EnabledCheck", conditionsArray);
    for (const TNTConditional& condition : enabledCheck) {
        json_object* conditionObject = json_object_new_object();
        condition.toJson(conditionObject);
        json_object_array_add(conditionsArray, conditionObject);
    }
}

void tnt::TNTOption::fromJson(json_object* object) {
    name = json_object_get_string(json_object_object_get(object, "Name"));
    id = json_object_get_int(json_object_object_get(object, "ID"));
    enabledCheck = std::vector<TNTConditional>{};
    json_object* conditionsArray = json_object_object_get(object, "EnabledCheck");
    if (conditionsArray != nullptr) {
        size_t conditionsCount = json_object_array_length(conditionsArray);
        for (int i = 0; i < conditionsCount; i++) {
            TNTConditional conditional {};
            conditional.fromJson(json_object_array_get_idx(conditionsArray, i));
            enabledCheck.push_back(conditional);
        }
    }
}

void tnt::TNTEvent::toJson(json_object* object) const {
    json_object_object_add(object, "Name", json_object_new_string(name.c_str()));
}

void tnt::TNTEvent::fromJson(json_object* object) {
    name = json_object_get_string(json_object_object_get(object, "Name"));
}

void tnt::TNTVariableEntry::toJson(json_object* object) const {
    json_object_object_add(object, "Name", json_object_new_string(name.c_str()));
    json_object_object_add(object, "Value", json_object_new_int(value));
}

void tnt::TNTVariableEntry::fromJson(json_object* object) {
    name = json_object_get_string(json_object_object_get(object, "Name"));
    value = json_object_get_int(json_object_object_get(object, "Value"));
}

void tnt::TNTConditional::toJson(json_object* object) const {
    json_object_object_add(object, "Variable", json_object_new_string(variable.c_str()));
    json_object_object_add(object, "Comparison", json_object_new_string(comparison.c_str()));
    json_object_object_add(object, "TargetValue", json_object_new_int(targetValue));
}

void tnt::TNTConditional::fromJson(json_object* object) {
    variable = json_object_get_string(json_object_object_get(object, "Variable"));
    targetValue = json_object_get_int(json_object_object_get(object, "TargetValue"));
    comparison = json_object_get_string(json_object_object_get(object, "Comparison"));
}
