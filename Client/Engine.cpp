#include "Engine.hpp"
#include "ExternalLibraryHeaders.h"
#include "Helper.h"

namespace TNAP {

	std::unique_ptr<Engine> Engine::s_instance{nullptr};

	Engine::Engine()
	{
	}

	Engine::~Engine()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Engine::init()
	{
		// Use the helper function to set up GLFW, GLEW and OpenGL
		m_window = Helpers::CreateGLFWWindow(1600, 900, "TNAP");
		if (!m_window)
			return;

		// Create an instance of the simulation class and initialise it
		// If it could not load, exit gracefully
		if (!simulation.Initialise())
		{
			glfwTerminate();
			return;
		}

		glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GLFW_TRUE);

#if USE_IMGUI
		m_TNAPImGui.init(m_window);
#endif
	}

	void Engine::update()
	{
		// Enter main GLFW loop until the user closes the window
		while (!glfwWindowShouldClose(m_window))
		{
#if USE_IMGUI
			m_TNAPImGui.beginRender();
#endif

			if (!simulation.Update(m_window))
				break;

#if USE_IMGUI
			bool showDemoWindow = true;
			ImGui::ShowDemoWindow(&showDemoWindow);

			m_TNAPImGui.endRender();
#endif

			// GLFW updating
			glfwSwapBuffers(m_window);
			glfwPollEvents();
		}
	}

	void Engine::sendMessage()
	{
	}

}
