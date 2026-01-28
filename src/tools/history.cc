#include "tools/history.h"

#include "imgui.h"

#include "app.h"

void tnt::tools::HistoryTool::doGui()
{
	uint32_t itemIndex = 0;
	for (tnt::HistoryItem& item : app->getHistory()) {
		ImGui::PushID(itemIndex);
        {
			if (ImGui::Button(item.displayName.c_str(), ImVec2(-1, 0))) {

			}
		}
		ImGui::PopID();
		itemIndex++;
	}
}
