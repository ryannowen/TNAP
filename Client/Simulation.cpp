#include "Simulation.h"

#include "ImGuiInclude.hpp"

std::shared_ptr<Helpers::Camera> Simulation::m_camera{ nullptr };

// Initialise this as well as the renderer, returns false on error
bool Simulation::Initialise()
{
	// Set up camera
	m_camera = std::make_shared<Helpers::Camera>();
	m_camera->Initialise(glm::vec3(0, 200, 900), glm::vec3(0)); // Jeep
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

	ImGui::Text(("FPS: " + std::to_string(1 / deltaTime)).c_str());

	ImGui::End();
#endif

	// The camera needs updating to handle user input internally
	m_camera->Update(window, deltaTime);

	return true;
}
