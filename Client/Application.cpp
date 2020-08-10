#include "Application.hpp"

#include <vector>

#include "Engine.hpp"
#include "GLFWCallBacks.hpp"
#include "LogMessage.hpp"
#include "Helper.h"

namespace TNAP {

	std::unique_ptr<Application> Application::s_instance{ nullptr };

	Application::Application()
	{
	}

	Application::~Application()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Application::init()
	{
		// Use the helper function to set up GLFW, GLEW and OpenGL
		m_window = Helpers::CreateGLFWWindow(static_cast<int>(m_windowSize.x), static_cast<int>(m_windowSize.y), "TNAP");

		if (!m_window)
		{
			LogMessage logMessage("Window Failed to create");
			logMessage.m_logType = LogMessage::ELogType::eError;

			getEngine()->sendMessage(&logMessage);
			return;
		}

		glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GLFW_TRUE);

		glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(this));

		glfwSetWindowPos(m_window, 160, 30);

		glfwSetDropCallback(m_window,
			[](GLFWwindow* argWindow, int argCount, const char** argPaths)
			{
				GLFWDropCallBackMessage message;

				for (unsigned int i = 0; i < argCount; ++i)
				{
					message.m_paths.reserve(i);

					message.m_paths.push_back(argPaths[i]);
				}

				TNAP::getEngine()->sendMessage(&message);

			});

	}

}