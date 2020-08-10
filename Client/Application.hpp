#pragma once

#include <memory>

#include "ExternalLibraryHeaders.h"

namespace TNAP {

	class Application
	{
	private:
		Application();
		static std::unique_ptr<TNAP::Application> s_instance;

		GLFWwindow* m_window;
		glm::vec2 m_windowSize{ 1600, 900 };



	public:
		~Application();
		inline static TNAP::Application* const getInstance()
		{
			if (!s_instance)
			{
				s_instance.reset(new TNAP::Application());
			}

			return s_instance.get();
		}

		void init();

		inline GLFWwindow* const getWindow() { return m_window; }

		inline void setWindowSize(const glm::vec2& argWindowSize) { m_windowSize = argWindowSize; }
		inline const glm::vec2& getWindowSize() const { return m_windowSize; }
	};

	inline Application* const getApplication() { return Application::getInstance(); }

}

