#include "SceneManager.hpp"

#include <iostream>

#include "Entity.hpp"
#include "Renderable.hpp"
#include "Scene.hpp"
#include "LogMessage.hpp"
#include "Engine.hpp"

#include "Light.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

#include "Application.hpp"

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
			m_scene = std::make_unique<Scene>("Our First Scene");

		int size = 10;

		for (int x = 0; x < size; x++)
		{
			for (int y = 0; y < size; y++)
			{
				for (int z = 0; z < size; z++)
				{
					m_scene->addEntity<Renderable>(false, "Apple"+std::to_string(x) + std::to_string(y) + std::to_string(z), "Apple\\apple.obj");
					m_scene->findEntity("Apple" + std::to_string(x) + std::to_string(y) + std::to_string(z))->getTransform().setScale({ 10, 10, 10 });
					m_scene->findEntity("Apple" + std::to_string(x) + std::to_string(y) + std::to_string(z))->getTransform().setTranslation({ x*3, y*3, z*3 });
				}
			}
		}

		//m_scene->loadFromFile("Our First Scene");

		/*m_scene->addEntity<Renderable>(false, "BirdPlane", "Primitives\\Plane.fbx");
		m_scene->addEntity<Renderable>(false, "4", "Primitives\\Cube.fbx");
		m_scene->findEntity("4")->addChild<Renderable>("4", "Primitives\\Cube.fbx");
		Entity* e = m_scene->findEntity("4_1");
		if (nullptr != e)
		{
			std::cout << "Setting Name: " << e->setName("4_") << std::endl;
			std::cout << "Setting Name: " << e->setName("4") << std::endl;
		}*/

		//Renderable* Hull{ m_scene->addEntity<Renderable>(false, "Hull", "AquaPig\\hull.obj") };

		//Renderable* Gunbase{ Hull->addChild<Renderable>("Gun_Base", "AquaPig\\gun_base.obj") };
		//Gunbase->getTransform().setTranslation({ 0.0f, 0.569f, -1.866f });

		//Renderable* Gun{ Gunbase->addChild<Renderable>("Gun_0", "AquaPig\\gun.obj") };
		//Gun->getTransform().setTranslation({ 0.0f, 1.506f, 0.644f });

		///*
		//for (int i = 0; i < 100; i++)
		//{
		//	Renderable* test{ m_scene->findEntity("Gun_"+std::to_string(i))->addChild<Renderable>("Gun_" + std::to_string(i+1), "AquaPig\\gun.obj") };
		//	test->getTransform().setTranslation({ 0.0f, i * 0.2f, 0.0f });
		//}
		//*/

		//Renderable* LeftWing{ Hull->addChild<Renderable>("LeftWing", "AquaPig\\wing_left.obj") };
		//LeftWing->getTransform().setTranslation({ 2.231f, 0.272f, -2.663f});

		//Renderable* RightWing{ Hull->addChild<Renderable>("LeftWing", "AquaPig\\wing_left.obj") };
		//RightWing->getTransform().setTranslation({ -2.231f, 0.272f, -2.663f});
		//RightWing->getTransform().setScale({ -1, 1, 1});

		//Renderable* Propeller{ Hull->addChild<Renderable>("Propeller", "AquaPig\\propeller.obj") };
		//Propeller->getTransform().setTranslation({ 0.0f, 1.395f, -3.616f});
		//Propeller->getTransform().setRotation({ 90.0f, 0.0f, 0.0f});
		

		//m_scene->destroyEntity("5");
		//m_scene->destroyEntity("Gun_Base");

		//m_scene->destroyEntity("4");

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

		static bool created{ false };

		if (!created && glfwGetKey(Application::getInstance()->getWindow(), GLFW_KEY_M))
		{
			m_scene->addEntity<SpotLight>(false, "First Light");
			created = true;
		}
	}

	void SceneManager::loadScene(const std::string& argFilePath)
	{
	}

	void SceneManager::unloadScene()
	{
	}

#if USE_IMGUI
	void SceneManager::imGuiRender()
	{
		m_scene->imGuiRender();
	}
#endif

}