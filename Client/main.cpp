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
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ExternalLibraryHeaders.h"
#include "Helper.h"
#include "Simulation.h"
#include "ImageLoader.h"

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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	GLuint textureBuffer;
	glGenTextures(1, &textureBuffer);
	glBindTexture(GL_TEXTURE_2D, textureBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	std::string textName("aqua_pig_2K.png");
	Helpers::ImageLoader image = Helpers::ImageLoader();
	if (!image.Load("Data\\Textures\\" + textName))
		std::cerr << textName << "does not exist Returning missing texture" << std::endl;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width(), image.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetData());

	ImVec2 imageSize(256, 256);

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

		ImGui::Begin("Image Test");

		ImGui::SliderFloat2("Texture Size", &imageSize.x, 64, 512);
		ImGui::Image((void*)(intptr_t)textureBuffer, imageSize);
		ImGui::SetWindowSize("Image Test", imageSize);
		
		ImGui::End();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		// GLFW updating
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up and exit
	ImGui_ImplOpenGL3_Shutdown();
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