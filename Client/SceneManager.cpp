#include "SceneManager.hpp"
#include "Scene.hpp"
#include "Entity.hpp"

#include <iostream>

namespace TNAP {

	std::unique_ptr<SceneManager> SceneManager::s_instance{ nullptr };

	SceneManager::SceneManager()
	{

	}

	SceneManager::~SceneManager()
	{
	}

	void SceneManager::init()
	{
		if (nullptr == m_scene)
			m_scene = std::make_unique<Scene>();

		m_scene->addEntity<Entity>("RANDOM NAME");
	}

	void SceneManager::update()
	{
	}

	void SceneManager::loadScene(const std::string& argFilePath)
	{
	}

	void SceneManager::unloadScene()
	{
	}

	TNAP::Scene* const SceneManager::getCurrentScene()
	{
		return m_scene.get();
	}

#if USE_IMGUI
	void SceneManager::imGuiRender()
	{
	}
#endif

}