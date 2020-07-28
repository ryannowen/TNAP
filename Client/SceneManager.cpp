#include "SceneManager.hpp"
#include "Scene.hpp"
#include "Entity.hpp"
#include "Renderable.hpp"

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

		m_scene->addEntity<Renderable>("3");
		m_scene->findEntity("3")->addChild<Renderable>("5");
		m_scene->findEntity("3")->findChild("5")->getTransform().setScale(glm::vec3(2));
		m_scene->findEntity("3")->findChild("5")->getTransform().setTranslation(glm::vec3(0,80,0));
		m_scene->addEntity<Renderable>("4");
		m_scene->findEntity("3")->getTransform().setTranslation(glm::vec3(200,0,200));
		m_scene->findEntity("3")->getTransform().setRotation(glm::vec3(0,0,0));
		//m_scene->findEntity("3")->getTransform().setRotation(glm::vec3(90,0,0));
	}

	void SceneManager::update()
	{
		m_scene->update();
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
		m_scene->imGuiRender();
	}
#endif

}