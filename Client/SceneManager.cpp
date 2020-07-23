#include "SceneManager.hpp"
#include "Scene.hpp"

namespace TNAP {

	SceneManager::~SceneManager()
	{
	}

	void SceneManager::init()
	{
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

}