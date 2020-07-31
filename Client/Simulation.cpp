#include "Simulation.h"

#include "ImGuiInclude.hpp"

std::shared_ptr<Helpers::Camera> Simulation::m_camera{ nullptr };

// Initialise this as well as the renderer, returns false on error
bool Simulation::Initialise()
{
	// Set up camera
	m_camera = std::make_shared<Helpers::Camera>();
	m_camera->Initialise(glm::vec3(0, 5, 10), glm::vec3(0), 60); // Jeep
	//m_camera->Initialise(glm::vec3(-13.82f, 5.0f, 1.886f), glm::vec3(0.25f, 1.5f, 0), 30.0f,0.8f); // Aqua pig

	// Set up renderer
	return true;
}

// Update the simulation (and render) returns false if program should close
bool Simulation::Update(GLFWwindow* window)
{
	// Calculate delta time since last called
	// We pass the delta time to the camera and renderer
	float timeNow = (float)glfwGetTime();
	float deltaTime{ timeNow - m_lastTime };
	m_lastTime = timeNow;

#if USE_IMGUI
	ImGui::Begin("Stats");
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("FPS average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	}
	ImGui::End();
#endif

	// The camera needs updating to handle user input internally
	m_camera->Update(window, deltaTime);

	return true;
}
