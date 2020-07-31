#include "SceneManager.hpp"
#include "Scene.hpp"
#include "Entity.hpp"
#include "Renderable.hpp"

#include "LogMessage.hpp"
#include "Engine.hpp"

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

		// Log Message tests
		LogMessage message("Adding Entity 3");
		TNAP::getEngine()->sendMessage(&message);
		message.m_message = "Adding Entity 4";
		TNAP::getEngine()->sendMessage(&message);

		m_scene->addEntity<Renderable>("BirdPlane", "Primitives\\Plane.fbx");
		m_scene->addEntity<Renderable>("4", "Primitives\\Cube.fbx");

		Renderable* Hull{ m_scene->addEntity<Renderable>("Hull", "AquaPig\\hull.obj") };

			Renderable* Gunbase{ Hull->addChild<Renderable>("Gun_Base", "AquaPig\\gun_base.obj") };
			Gunbase->getTransform().setTranslation({ 0.0f, 0.569f, -1.866f });

				Renderable* Gun{ Gunbase->addChild<Renderable>("Gun", "AquaPig\\gun.obj") };
				Gun->getTransform().setTranslation({ 0.0f, 1.506f, 0.644f });

		Renderable* LeftWing{ Hull->addChild<Renderable>("LeftWing", "AquaPig\\wing_left.obj") };
		LeftWing->getTransform().setTranslation({ 2.231f, 0.272f, -2.663f});

		Renderable* RightWing{ Hull->addChild<Renderable>("LeftWing", "AquaPig\\wing_left.obj") };
		RightWing->getTransform().setTranslation({ -2.231f, 0.272f, -2.663f});
		RightWing->getTransform().setScale({ -1, 1, 1});

		Renderable* Propeller{ Hull->addChild<Renderable>("Propeller", "AquaPig\\propeller.obj") };
		Propeller->getTransform().setTranslation({ 0.0f, 1.395f, -3.616f});
		Propeller->getTransform().setRotation({ 90.0f, 0.0f, 0.0f});

		//static_cast<Renderable*>(m_scene->findEntity("3"))->loadModel();
		/*
		m_scene->findEntity("3")->addChild<Renderable>("5", "low-poly-spider-tank\\Part_01xxx.fbx");
		m_scene->findEntity("3")->findChild("5")->getTransform().setScale(glm::vec3(2));
		m_scene->findEntity("3")->findChild("5")->getTransform().setTranslation(glm::vec3(0,80,0));
		m_scene->addEntity<Renderable>("4", "low-poly-spider-tank\\Part_01xxx.fbx");
		m_scene->findEntity("3")->getTransform().setTranslation(glm::vec3(200,0,200));
		m_scene->findEntity("3")->getTransform().setRotation(glm::vec3(0,0,0));
		//m_scene->findEntity("3")->getTransform().setRotation(glm::vec3(90,0,0));
		*/
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