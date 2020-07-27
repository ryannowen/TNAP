#pragma once

#if USE_IMGUI

#include "ImGuiInclude.hpp"

class GLFWwindow;

namespace TNAP {

	class TNAPImGui
	{
	private:
		ImGuiIO* io;

	public:
		TNAPImGui();
		~TNAPImGui();
		void init();
		void beginRender();
		void endRender();
	};

}

#endif