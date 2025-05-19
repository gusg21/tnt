//
// TNT (Triage Narrative Tool) created by 5' Digital. 
// All rights reserved.
//

#pragma once

#include <vector>

#include <SDL3/SDL.h>

#include "tool.h"
#include "tnt_data.h"

namespace tnt {

	class HistoryItem {
	public:
		std::string displayName;
		tnt::TNTData data;
	};

	class App {
	public:
		App();
		~App();
		App(const App&) = delete;
		App& operator=(const App&) = delete;

		void openTool(Tool* tool);
		void closeTool(Tool* tool);
		const std::vector<Tool*>& getTools();
		void focusTool(Tool* tool);
		void handleSdlEvent(SDL_Event event);
		void doFrame(float deltaTime);
		bool wantsExit();
		void changeID(int oldToolId, int newToolId);
		void saveState(const std::string& displayName);
		void undo();
		void redo();
		void clearHistory();
		std::vector<HistoryItem>& getHistory();

		tnt::TNTData& getTntData();
		const std::string& getLastJsonPath();
		void setLastJsonPath(std::string& path);
        bool isDataDirty();
        void resetDataDirtyFlag();

	private:
		SDL_Window* m_window;
		SDL_GLContext m_context;
		bool m_wantsExit;
		std::vector<Tool*> m_tools;
		std::vector<Tool*> m_newTools{};
		std::vector<Tool*> m_toolsToClose{};
		tnt::TNTData m_tntData;
		Tool* m_toolToFocus;
		bool m_showImGuiDemo = false;
		bool m_fullscreen = false;
		std::string m_lastJsonPath{};
		std::vector<HistoryItem> m_history{};
		uint32_t m_currentHistoryIndex;
        bool m_dataDirty;
	};

};

