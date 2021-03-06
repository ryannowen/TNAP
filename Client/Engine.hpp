#pragma once

#include <memory>
#include <vector>

#include "System.hpp"
#include "TNAPImGui.hpp"
#include "Simulation.h"

struct GLFWwindow;

namespace TNAP {

	class Engine
	{
	private:
		Engine();
		static std::unique_ptr<TNAP::Engine> s_instance;
		std::vector<std::unique_ptr<TNAP::System>> m_systems;

#if USE_IMGUI
		TNAPImGui m_TNAPImGui;
#endif
		Simulation simulation;

	public:
		~Engine();
		inline static TNAP::Engine* const getInstance()
		{
			if (!s_instance)
			{
				s_instance.reset(new TNAP::Engine());
			}

			return s_instance.get();
		}

		void init();
		void update();
		void sendMessage(TNAP::Message* const argMessage);
	};

	inline Engine* const getEngine() { return Engine::getInstance(); }

}

