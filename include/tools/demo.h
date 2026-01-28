#pragma once

#include "tool.h"

namespace tnt {
	namespace tools {

		class DemoTool : public tnt::Tool {
		public:
			explicit DemoTool(tnt::App* app) : tnt::Tool(app, "Dear ImGui Demo", "") {}

			void doGui() override;
		};

	}
}