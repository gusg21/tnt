#pragma once

#include "ims_icons.h"

#include "tool.h"

namespace tnt {
	namespace tools {
		class HistoryTool : public tnt::Tool {
		public:
			explicit HistoryTool(tnt::App* app) : tnt::Tool(app, "History", ICON_MS_HISTORY) {}

			void doGui() override;
		};
	};
};