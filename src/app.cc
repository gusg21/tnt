//
// TNT (Triage Narrative Tool) created by 5' Digital. 
// All rights reserved.
//

#include "app.h"

#include <stdexcept>

#include "imgui.h"
#include "imnodes.h"
#include "glad/glad.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"
#include "ims_icons.h"

#include "tools/demo.h"
#include "tools/graph_view.h"
#include "tools/history.h"

using namespace tnt;

ImWchar editor_renderer_mdi_icon_ranges[3] = { ICON_MIN_MS, ICON_MAX_MS, 0 };

tnt::App::App() {
	// Create window.
	m_window = SDL_CreateWindow(
		"TNT", 1024, 768,
		(m_fullscreen ? SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS : 0) | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY |
		SDL_WINDOW_OPENGL);
	if (m_window == NULL) {
		throw std::runtime_error("Failed to create SDL window!");
	}

	// Load OpenGL.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	m_context = SDL_GL_CreateContext(m_window);
	if (m_context == NULL) {
		throw std::runtime_error("Failed to acquire OpenGL context!");
	}
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	// Setup ImGui.
	ImGui::CreateContext();
	ImNodes::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("res/SchibstedGrotesk-Regular.ttf", 16);
	auto config = new ImFontConfig();
	config->GlyphOffset = ImVec2(0, 3);
	config->MergeMode = true;
	io.Fonts->AddFontFromFileTTF("res/msi.ttf", 20, config, editor_renderer_mdi_icon_ranges);
	io.FontGlobalScale = 1.0f;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsLight();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;

	ImGui_ImplSDL3_InitForOpenGL(m_window, m_context);
	ImGui_ImplOpenGL3_Init(nullptr);

	// Misc.
	m_wantsExit = false;
	m_tools = std::vector<tnt::Tool*>{ };
	//    openTool(new tnt::tools::DemoTool(this));
    openTool(new tnt::tools::GraphViewTool(this));
//    openTool(new tnt::tools::HistoryTool(this));

	m_tntData = tnt::TNTData{ };

	m_lastJsonPath = SDL_GetBasePath();

	clearHistory(); // I just want to be revision 500 :)
}

tnt::App::~App() {
	ImNodes::DestroyContext();
	ImGui::DestroyContext();

	SDL_GL_DestroyContext(m_context);
	SDL_DestroyWindow(m_window);
}

void tnt::App::openTool(Tool* tool) {
	m_newTools.push_back(tool);
}

void tnt::App::closeTool(Tool* tool) {
	m_toolsToClose.push_back(tool);
}

void tnt::App::handleSdlEvent(SDL_Event event) {
	ImGui_ImplSDL3_ProcessEvent(&event);

	switch (event.type) {
	case SDL_EVENT_QUIT:
		m_wantsExit = true;
		break;
	}
}

void tnt::App::doFrame(float deltaTime) {
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplSDL3_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();

	if (m_showImGuiDemo)
		ImGui::ShowDemoWindow();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Window")) {
			if (ImGui::MenuItem("Show History", "", false)) {
				openTool(new tnt::tools::HistoryTool(this));
			}

			if (ImGui::MenuItem("Fullscreen", "", &m_fullscreen)) {
				SDL_SetWindowFullscreen(m_window, m_fullscreen);
				if (!m_fullscreen) {
					SDL_SetWindowBordered(m_window, true);
					SDL_SetWindowSize(m_window, 1024, 768);
					ImGui::GetMainViewport()->Size.x = 1024;
					ImGui::GetMainViewport()->Size.y = 768;
				}
				else {
					SDL_SetWindowBordered(m_window, false);
				}
			}
			if (ImGui::MenuItem("Quit")) {
				m_wantsExit = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options")) {
			if (ImGui::MenuItem("Show ImGui Demo Window", nullptr, &m_showImGuiDemo)) {
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}


	for (Tool* tool : m_tools) {
		bool shouldOpenAboutPopup = false;

		if (tool->isBig()) {
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_Appearing);
			ImGui::SetNextWindowViewport(viewport->ID);
		}
		else {
			ImGui::SetNextWindowSize(ImVec2(900, 650), ImGuiCond_Appearing);
		}
		if (tool == m_toolToFocus) {
			ImGui::SetNextWindowFocus();
			m_toolToFocus = nullptr;
		}
		bool toolOpen = true;
		if (ImGui::Begin((tool->icon + " " + tool->name + "##" + std::to_string((intptr_t)tool)).c_str(), &toolOpen,
			ImGuiWindowFlags_MenuBar
		)) {
			if (ImGui::BeginMenuBar()) {
				tool->doMenuGui();

				if (ImGui::BeginMenu("Tool")) {
					if (ImGui::MenuItem("About...", nullptr, nullptr)) {
						// Show tool about page.
						shouldOpenAboutPopup = true;
					}

					ImGui::EndMenu();
				}


				ImGui::EndMenuBar();
			}
			tool->doGui();

			if (shouldOpenAboutPopup) {
				ImGui::OpenPopup("About Page");
			}

			if (ImGui::BeginPopupModal("About Page", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("%s", tool->getAboutPage().c_str());

				if (ImGui::Button("Close")) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
		ImGui::End();

		if (!toolOpen) {
			closeTool(tool);
		}
	}

	if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_Z)) {
        if (ImGui::IsKeyDown(ImGuiKey_ModShift)) {
            redo();
        }
        else {
            undo();
        }
	}

	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	glClearColor(0.45f, 0.55f, 0.60f, 0.00f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
	SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

	SDL_GL_SwapWindow(m_window);

	for (Tool*& tool : m_newTools) {
		m_tools.push_back(tool);
		tool->open();
	}
	m_newTools.clear();

	std::vector<int> toolIndices{};
	for (Tool*& closeTool : m_toolsToClose) {
		int index = 0;
		for (Tool* tool : m_tools) {
			if (closeTool == tool) {
				toolIndices.push_back(index);
			}
			index++;
		}
	}
	for (int index : toolIndices) {
		m_tools.erase(m_tools.begin() + index);
	}
	m_toolsToClose.clear();
}

bool tnt::App::wantsExit() {
	return m_wantsExit;
}

void tnt::App::changeID(int oldToolId, int newToolId)
{
}

void tnt::App::saveState(const std::string& displayName)
{
	HistoryItem item = HistoryItem{};
	item.data = m_tntData;
	item.displayName = displayName;

	m_history.push_back(item);

	m_currentHistoryIndex++;
}

void tnt::App::undo()
{
	if (m_currentHistoryIndex == 0) {
		return;
	}

	if (m_currentHistoryIndex == m_history.size()) {
		saveState("Undo Anchor"); // Keep an end state.
        m_currentHistoryIndex--;
	}

	m_currentHistoryIndex--;

	HistoryItem& item = m_history[m_currentHistoryIndex];
	m_tntData = item.data;

    m_dataDirty = true;
}

void tnt::App::redo()
{
	if (m_currentHistoryIndex < m_history.size() - 1) {
		m_currentHistoryIndex++;

		HistoryItem& item = m_history[m_currentHistoryIndex];
		m_tntData = item.data;

        m_dataDirty = true;
	}
}

void tnt::App::clearHistory()
{
	m_history.clear();
	m_currentHistoryIndex = 0;
}

std::vector<HistoryItem>& tnt::App::getHistory()
{
	return m_history;
}

tnt::TNTData& App::getTntData() {
	return m_tntData;
}

const std::string& tnt::App::getLastJsonPath()
{
	return m_lastJsonPath;
}

void tnt::App::setLastJsonPath(std::string& path)
{
	m_lastJsonPath = path;
}

const std::vector<Tool*>& App::getTools() {
	return m_tools;
}

void App::focusTool(Tool* tool) {
	m_toolToFocus = tool;
}

bool App::isDataDirty() {
    return m_dataDirty;
}

void App::resetDataDirtyFlag() {
    m_dataDirty = false;
}
