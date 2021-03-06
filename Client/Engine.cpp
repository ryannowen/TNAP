#include "Engine.hpp"

#include "ExternalLibraryHeaders.h"
#include "Helper.h"

#include "Application.hpp"
#include "SceneManager.hpp"
#include "Renderer3D.hpp"
#include "Logger.hpp"

namespace TNAP {

	std::unique_ptr<Engine> Engine::s_instance{nullptr};

	Engine::Engine()
	{
	}

	Engine::~Engine()
	{

	}

	void Engine::init()
	{
		getApplication()->init();

		// Logger system
		m_systems.push_back(std::make_unique<TNAP::Logger>());
		m_systems.back()->init();

		// Renderer3D system
		m_systems.push_back(std::make_unique<TNAP::Renderer3D>());
		m_systems.back()->init();


		TNAP::getSceneManager()->init();

		// Create an instance of the simulation class and initialise it
		// If it could not load, exit gracefully
		if (!simulation.Initialise())
		{
			glfwTerminate();
			return;
		}

#if USE_IMGUI
		m_TNAPImGui.init();
#endif
	}

	void Engine::update()
	{
		GLFWwindow* const window{ getApplication()->getWindow() };
		// Enter main GLFW loop until the user closes the window
		while (!glfwWindowShouldClose(window))
		{
#if USE_IMGUI
			m_TNAPImGui.beginRender();
#endif

			TNAP::getSceneManager()->update();

			for (std::unique_ptr<System>& system : m_systems)
			{
				if (system->getEnabled())
						system->update();
			}

			if (!simulation.Update(window))
				break;

#if USE_IMGUI
			/*bool showDemoWindow = true;
			ImGui::ShowDemoWindow(&showDemoWindow);*/

			TNAP::getSceneManager()->imGuiRender();

			for (std::unique_ptr<TNAP::System>& system : m_systems)
			{
				if (system->getEnabled())
					system->imGuiRender();
			}

			m_TNAPImGui.endRender();
#endif

			// GLFW updating
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	void Engine::sendMessage(TNAP::Message* const argMessage)
	{
		for (std::unique_ptr<TNAP::System>& system : m_systems)
		{
			if (system->getEnabled())
				system->sendMessage(argMessage);
		}

	}

}
