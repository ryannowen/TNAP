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

		//m_scene->loadFromFile("Our First Scene");
		
		int size{ 5 };
		for (int x = 0; x < size; x++)
		{
			for (int y = 0; y < size; y++)
			{
				for (int z = 0; z < size; z++)
				{
					Renderable* newGun{ m_scene->addEntity<Renderable>(false, "Gun_" + std::to_string(x * 10) + std::to_string(y * 10) + std::to_string(z * 10), "AR.fbx") };
					newGun->getTransform().setScale(glm::vec3(0.1));
					newGun->getTransform().setTranslation({ x * 10, y * 10, z * 10 });
					newGun->getTransform().setRotation({ rand() % 360, rand() % 360, rand() % 360 });
				}
			}
		}

		Light* const light{ m_scene->addEntity<Light>(false, "LIGHT") };
		light->getTransform().setRotation(glm::vec3(0, -90, 0));

		//m_scene->addEntity<PointLight>(false, "LIGHT");
		//m_scene->addEntity<SpotLight>(false, "LIGHT");

		/*//Renderable* Hull{ m_scene->addEntity<Renderable>(false, "Hull", "AquaPig\\hull.obj") };

		//Renderable* Gunbase{ Hull->addChild<Renderable>("Gun_Base", "AquaPig\\gun_base.obj") };
		//Gunbase->getTransform().setTranslation({ 0.0f, 0.569f, -1.866f });

		//Renderable* Gun{ Gunbase->addChild<Renderable>("Gun_0", "AquaPig\\gun.obj") };
		//Gun->getTransform().setTranslation({ 0.0f, 1.506f, 0.644f });

		//Renderable* LeftWing{ Hull->addChild<Renderable>("LeftWing", "AquaPig\\wing_left.obj") };
		//LeftWing->getTransform().setTranslation({ 2.231f, 0.272f, -2.663f});

		//Renderable* RightWing{ Hull->addChild<Renderable>("LeftWing", "AquaPig\\wing_left.obj") };
		//RightWing->getTransform().setTranslation({ -2.231f, 0.272f, -2.663f});
		//RightWing->getTransform().setScale({ -1, 1, 1});

		//Renderable* Propeller{ Hull->addChild<Renderable>("Propeller", "AquaPig\\propeller.obj") };
		//Propeller->getTransform().setTranslation({ 0.0f, 1.395f, -3.616f});
		//Propeller->getTransform().setRotation({ 90.0f, 0.0f, 0.0f});
		*/		
	}

	void SceneManager::update()
	{
		m_scene->update();

		static bool created{ false };

		if (!created && glfwGetKey(Application::getInstance()->getWindow(), GLFW_KEY_M))
		{
			m_scene->addEntity<PointLight>(false, "First Light");
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