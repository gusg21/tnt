#pragma once

#include <string>

namespace tnt {
    class App; // Forward declare.

	class Tool {
	public:
		Tool(tnt::App* app, std::string name, std::string icon);
        Tool(const Tool&) = delete;
        Tool& operator=(const Tool&) = delete;

		virtual void open();
		virtual void doGui();
		virtual void doMenuGui();
		virtual void close();
        virtual bool isEditingEntry(uint32_t id);
        virtual bool isBig();
        virtual std::string getAboutPage();

        tnt::App* app;
		std::string name;
		std::string icon;
	};

}