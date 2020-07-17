/* 
	main.cpp : This file contains the 'main' function. Program execution begins and ends there.

	A set of helper classes and start files for the 3D Graphics Programming module
	
	This project uses a number of helper libraries contained in the External folder within the project directory
	The ExternalLibraryHeaders.h loads their headers

	Visual Studio Project Setup
		The paths for the headers are set in project properties / C/C++ / General
		The paths for the libraries are set in project properties / Linker / General
		The static libraries to link to are listed in Linker / Input
		There are also some runtime DLLs required. These are in the External/Bin directory. In order for Viz to find these
		during debugging the Debugging / environment is appended with the correct path
		If you run the exe outside of Viz these dlls need to be in the same folder as the exe but note that the provided
		MakeDistributable.bat batch file automatically copies them into the correct directory for you

	Keith ditchburn 2019
*/

#include "ExternalLibraryHeaders.h"
#include "Helper.h"
#include "Simulation.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

int main()
{
	// Use the helper function to set up GLFW, GLEW and OpenGL
	GLFWwindow* window{ Helpers::CreateGLFWWindow(1600, 900, "Simple example") };
	if (!window)
		return -1;

	// Create an instance of the simulation class and initialise it
	// If it could not load, exit gracefully
	Simulation simulation;
	if (!simulation.Initialise())
	{
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
	ImGui::StyleColorsDark();

	bool show_demo_window = true;

	// Enter main GLFW loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		if (!simulation.Update(window))
			break;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		// GLFW updating
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up and exit
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

//int main()
//{
//	// Use the helper function to set up GLFW, GLEW and OpenGL
//	GLFWwindow* window{ Helpers::CreateGLFWWindow(1024, 768, "Simple example") };
//	if (!window)
//		return -1;
//
//	// Create an instance of the simulation class and initialise it
//	// If it could not load, exit gracefully
//	Simulation simulation;	
//	if (!simulation.Initialise())
//	{
//		glfwTerminate();
//		return -1;
//	}
//
//	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
//
//	// Enter main GLFW loop until the user closes the window
//	while (!glfwWindowShouldClose(window))
//	{				
//		if (!simulation.Update(window))
//			break;
//		
//		// GLFW updating
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	// Clean up and exit
//	glfwDestroyWindow(window);
//	glfwTerminate();
//
//	return 0;
//}