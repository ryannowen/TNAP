#pragma once

#include "System.hpp"
#include <memory>
#include <string>

namespace TNAP {

	class Scene;

	class SceneManager: public TNAP::System
	{
	private:
		SceneManager();
		static std::unique_ptr<TNAP::SceneManager> s_instance;
		std::unique_ptr<TNAP::Scene> m_scene;

	public:
		~SceneManager();
		inline static TNAP::SceneManager* const getInstance()
		{
			if (!s_instance)
			{
				s_instance.reset(new TNAP::SceneManager());
			}

			return s_instance.get();
		}

		virtual void init() override;
		virtual void update() override;
		void loadScene(const std::string& argFilePath);
		void unloadScene();
		inline TNAP::Scene* const getCurrentScene();
	};

	inline TNAP::SceneManager* const getSceneManager() { return TNAP::SceneManager::getInstance(); }

}
