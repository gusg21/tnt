#include "tool.h"

tnt::Tool::Tool(tnt::App* app, std::string name, std::string icon)
{
    this->app = app;
	this->name = name;
	this->icon = icon;
}

void tnt::Tool::open() { }

void tnt::Tool::doGui() { }

void tnt::Tool::doMenuGui() { }

void tnt::Tool::close() { }

bool tnt::Tool::isEditingEntry(uint32_t id) {
    return false;
}

bool tnt::Tool::isBig() {
    return false;
}

std::string tnt::Tool::getAboutPage() {
    return "Tool \"" + name + "\"";
}
