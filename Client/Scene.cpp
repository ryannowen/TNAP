#include "Scene.hpp"

#include <fstream>

#include "Entity.hpp"
#include "Engine.hpp"
#include "ImGuiInclude.hpp"
#include "Renderable.hpp"
#include "Renderer3D.hpp"
#include "Application.hpp"
#include "Utilities.hpp"
#include "GetMaterialHandleMessage.hpp"

#include "Light.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

namespace TNAP {

	Scene::Scene(const std::string& argName)
		:	m_sceneName(argName)
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::init()
	{
	}

	void Scene::update()
	{

		//Transform transform;

		for (const size_t entityHandle : m_parentHandles)
		{
			if (m_entities.at(entityHandle)->getEnabled())
			{
				m_entities.at(entityHandle)->update(glm::mat4(1));
			}
		}
	}

	void Scene::saveScene()
	{
		// TODO : Save recursive from parent handles

		std::ofstream outputFile;
		// Save scene contents
		outputFile.open("Data/SaveLoad/" + m_sceneName + ".csv");

		for (const std::shared_ptr<Entity>& entity : m_entities)
		{
			entity->saveData(outputFile);
		}

		outputFile.close();

		// Save scene properties
		outputFile.open("Data/SaveLoad/" + m_sceneName + " Properties.csv");

		outputFile << m_ambientColour.x << " " << m_ambientColour.y << " " << m_ambientColour.z << ",";
		outputFile << m_ambientIntensity << "," << m_exposure << "," << m_gamma;
		outputFile << std::endl;

		outputFile.close();

	}

	void Scene::loadFromFile(const std::string& argFilePath)
	{
#if USE_IMGUI
		Entity::setSelected(nullptr);
#endif
		// Delete all objects in current scene
		m_mapEntities.clear();
		m_entities.clear();
		m_parentHandles.clear();

		// Load from save file
		std::ifstream savedScene;
		std::string info;


		// Load scene properties
		/*
		0 Ambient colour
		1 Ambient intensity
		2 exposure
		3 gamma
		*/
		savedScene.open("Data/SaveLoad/" + argFilePath + " Properties.csv");

		while (std::getline(savedScene, info))
		{
			// Seperate info
			std::vector<std::string> sceneInfo = TNAP::stringToVector<std::string>(info, ",", [](const std::string& str) { return str; });

			// Ambient colour
			std::vector<float> ambientColour = TNAP::stringToVector<float>(sceneInfo.at(0), " ", [](const std::string& str) { return std::stof(str); }, 3);
			m_ambientColour = glm::vec3(ambientColour.at(0), ambientColour.at(1), ambientColour.at(2));

			// Ambient intensity
			m_ambientIntensity = std::stof(sceneInfo.at(1));

			// Exposure
			m_exposure = std::stof(sceneInfo.at(2));

			// Gamma
			m_gamma = std::stof(sceneInfo.at(3));
		}

		savedScene.close();
		info.clear();

		// Load scene contents
		/*
		0 name;
		1 entityType;
		2 enabled;
		3 hasParent;
		4 parentHandle;
		5 transform;
		6 children;

		7 modelPath;
		8 materialNames;

		7 light colour
		8 light intensity
		9 light range
		10 light fov
		*/
		savedScene.open("Data/SaveLoad/" + argFilePath + ".csv");

		while (std::getline(savedScene, info))
		{
			// Seperate info
			std::vector<std::string> entityInfo = TNAP::stringToVector<std::string>(info, ",", [](const std::string& str) { return str; });

			EEntityType type = static_cast<EEntityType>(std::stoi(entityInfo.at(1)));

			switch (type)
			{
			case EEntityType::eRenderable:
			{
				Renderable* newRenderable = addEntity<Renderable>(std::stoi(entityInfo.at(3)), entityInfo.at(0), entityInfo.at(7));

				// Materials
				std::vector<std::string> materials = TNAP::stringToVector<std::string>(entityInfo.at(8), " ", [](const std::string& str) { return str; });
				std::vector<size_t> materialHandles;
				for (const std::string& materialName : materials)
				{
					GetMaterialHandleMessage message;
					message.m_materialName = materialName;
					TNAP::getEngine()->sendMessage(&message);
					materialHandles.push_back(message.m_materialHandle);
				}
				newRenderable->setMaterialHandles(materialHandles);
			}
			break;

			case EEntityType::eLight:
			{
				Light* newLight = addEntity<Light>(std::stoi(entityInfo.at(3)), entityInfo.at(0));
				
				std::vector<float> lightColour = TNAP::stringToVector<float>(entityInfo.at(7), " ", [](const std::string& str) { return std::stof(str); }, 3);
				newLight->setColour({lightColour.at(0), lightColour.at(1), lightColour.at(2)});

				newLight->setIntensity(std::stof(entityInfo.at(8)));
			}
			break;

			case EEntityType::ePointLight:
			{
				PointLight* newLight = addEntity<PointLight>(std::stoi(entityInfo.at(3)), entityInfo.at(0));

				std::vector<float> lightColour = TNAP::stringToVector<float>(entityInfo.at(7), " ", [](const std::string& str) { return std::stof(str); }, 3);
				newLight->setColour({ lightColour.at(0), lightColour.at(1), lightColour.at(2) });

				newLight->setIntensity(std::stof(entityInfo.at(8)));

				newLight->setRange(std::stof(entityInfo.at(9)));
			}
			break;

			case EEntityType::eSpotLight:
			{
				SpotLight* newLight = addEntity<SpotLight>(std::stoi(entityInfo.at(3)), entityInfo.at(0));

				std::vector<float> lightColour = TNAP::stringToVector<float>(entityInfo.at(7), " ", [](const std::string& str) { return std::stof(str); }, 3);
				newLight->setColour({ lightColour.at(0), lightColour.at(1), lightColour.at(2) });

				newLight->setIntensity(std::stof(entityInfo.at(8)));

				newLight->setRange(std::stof(entityInfo.at(9)));

				newLight->setFov(std::stof(entityInfo.at(10)));
			}
			break;

			default:
				Entity* newEntity = addEntity<Entity>(std::stoi(entityInfo.at(3)), entityInfo.at(0));
				break;
			}

			// Is Enabled
			m_entities.back()->setEnabled(std::stoi(entityInfo.at(2)));

			// Parent Handle
			if (std::stoi(entityInfo.at(3)))
				m_entities.back()->setParentHandle(std::stoull(entityInfo.at(4)));

			// Transform
			std::vector<float> transformValues = TNAP::stringToVector<float>(entityInfo.at(5), " ", [](const std::string& str) { return std::stof(str); }, 9);
			m_entities.back()->getTransform().setTranslation(glm::vec3(transformValues.at(0), transformValues.at(1), transformValues.at(2)));
			m_entities.back()->getTransform().setRotation(glm::vec3(transformValues.at(3), transformValues.at(4), transformValues.at(5)));
			m_entities.back()->getTransform().setScale(glm::vec3(transformValues.at(6), transformValues.at(7), transformValues.at(8)));

			// Children
			if (entityInfo.size() >= 7 && "" != entityInfo.at(6))
			{
				std::vector<size_t> childrenHandles = TNAP::stringToVector<size_t>(entityInfo.at(6), " ", [](const std::string& str) { return std::stof(str); });
				m_entities.back()->setChildren(childrenHandles);
			}
		}

		savedScene.close();
	}

	TNAP::Entity* const Scene::findEntity(const std::string& argName)
	{
		if (m_mapEntities.find(argName) == m_mapEntities.end())
			return nullptr;

		return m_entities[m_mapEntities.at(argName)].get();
	}

	TNAP::Entity* const Scene::findEntity(const size_t argHandle)
	{
		if (argHandle >= 0 && argHandle < m_entities.size())
			return m_entities[argHandle].get();

		return nullptr;
	}

	// bool = was entity found, size_t = index / handle
	const std::pair<bool, size_t> Scene::getEntityIndex(const std::string& argName)
	{
		// Invalid name
		if (m_mapEntities.find(argName) == m_mapEntities.end())
			return std::pair<bool, size_t>(false, 0);

		return std::pair<bool, size_t>(true, m_mapEntities.at(argName));
	}

	const bool Scene::entityExists(const std::string& argName)
	{
		// Entity doesnt exist
		if (m_mapEntities.find(argName) == m_mapEntities.end())
			return false;
		return true;
	}

	bool Scene::updateEntityInMap(const std::string& argOldName, const std::string& argNewName, const size_t argHandle)
	{
		// Entity doesnt exist
		if (m_mapEntities.find(argOldName) == m_mapEntities.end())
			return false;

		// New name already exists
		if (m_mapEntities.find(argNewName) != m_mapEntities.end())
			return false;

		m_mapEntities.erase(argOldName);
		m_mapEntities.insert({argNewName, argHandle});
		return true;
	}

	void Scene::destroyEntity(const std::string& argName)
	{
		// Invalid entity name
		if (m_mapEntities.find(argName) == m_mapEntities.end())
			return;

		// Get index of entity to be deleted
		size_t entityIndex = m_mapEntities.at(argName);

		destroyEntity(entityIndex);
	}

	void Scene::destroyEntity(const size_t argHandle)
	{
		// Invalid entity handle
		if (argHandle >= m_entities.size() || argHandle < 0)
			return;

		std::string entityName = m_entities.at(argHandle)->getName(); // Store name of entity since the entity handle could change
		m_entities.at(argHandle)->deleteChildren();

		if (!getEntityIndex(entityName).first) // If the entity doesnt exists
			return;

		size_t index{ getEntityIndex(entityName).second }; // Get entity index / handle 

		if (m_entities.at(index)->getHasParent()) // Does the entity have a parent?
		{
			size_t parentHandle = m_entities.at(index)->getParentHandle();
			m_entities.at(parentHandle)->removeChild(index); // Remove current entity handle from its parent's children vector
		}
		else // Entity doesnt have a parent, meaning it is a top level parent
		{
			size_t parentHandleIndex = m_entities.at(index)->getParentHandle(); // Get the index of the current entity in parent handles
			m_entities.at(m_parentHandles.back())->setParentHandle(parentHandleIndex);
			std::swap(m_parentHandles.at(parentHandleIndex), m_parentHandles.back());
			m_entities.at(index)->setParentHandle(m_parentHandles.size()-1);
		}

		// Entity at the back of the vector will be swapped with the current entity to be deleted
		m_entities.back()->setHandle(index); // Update the handle
		m_entities.back()->updateChildrenParentHandles(); // Update children parent handles
		m_mapEntities.at(m_entities.back()->getName()) = index; // Update entity index stored in the map
		
		if (m_entities.back()->getHasParent()) // If entity at back of vector is a top level entity
		{
			size_t parentHandleIndex = m_entities.back()->getParentHandle();
			m_entities.at(parentHandleIndex)->replaceChildHandle(m_entities.size()-1, index); // Replace the child handle in parents children vector with new handle
		}
		else // Entity to be swapped with doesnt have a parent, meaning it is a top level entity
		{
			m_parentHandles.at(m_entities.back()->getParentHandle()) = index; // Update handle stored in parent handles
		}

		std::swap(m_entities.at(index), m_entities.back()); // Swap the entity to delete with the entity at the back of the vector
		
#if USE_IMGUI
		Entity::setSelected(nullptr);
#endif
		m_mapEntities.erase(m_entities.back()->getName()); // Erase entity from the map
		if (!m_entities.back()->getHasParent())
			m_parentHandles.pop_back();
		m_entities.pop_back();
	}

	void Scene::sendShaderData(const GLuint argProgram) const
	{
		GLuint ambientColour_id = glGetUniformLocation(argProgram, "sceneData.m_ambientColour");
		glUniform3fv(ambientColour_id, 1, glm::value_ptr(m_ambientColour));

		GLuint ambientIntensity_id = glGetUniformLocation(argProgram, "sceneData.m_ambientIntensity");
		glUniform1f(ambientIntensity_id, m_ambientIntensity);

		GLuint exposure_id = glGetUniformLocation(argProgram, "sceneData.m_exposure");
		glUniform1f(exposure_id, m_exposure);

		GLuint gamma_id = glGetUniformLocation(argProgram, "sceneData.m_gamma");
		glUniform1f(gamma_id, m_gamma);
	}

	void Scene::sendMessage(TNAP::Message* const argMessage)
	{
	}

#if USE_IMGUI
	void Scene::imGuiRender()
	{
		static bool showHierarchy{ true };
		ImGui::Begin("Hierarchy", &showHierarchy, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Create"))
				{
					static std::string newName;
					static std::string filepath;
					static bool newEntityEnabled{ true };

					// Clear entity selection
					if (ImGui::Button("Clear Selected"))
					{
						Entity::setSelected(nullptr);
					}

					ImGui::Separator();

					ImGui::InputText("Entity Name", &newName);
					ImGui::Checkbox("Enabled", &newEntityEnabled);

					ImGui::Separator();

					// Empty Entity
					if (ImGui::Button("Create Empty"))
					{
						Entity* newEntity = nullptr;

						if (nullptr == Entity::getSelected())
							newEntity = addEntity<Entity>(false, newName);
						else
							newEntity = Entity::getSelected()->addChild<Entity>(newName);
						newEntity->setEnabled(newEntityEnabled);
						newName = "";
					}

					ImGui::Separator();

					// Primitive
					if (ImGui::BeginMenu("Primitive"))
					{
						bool shouldCreate = false;

						if (ImGui::Button("Cube"))
						{
							shouldCreate = true;
							filepath = "Primitives\\Cube.fbx";
						}
						if (ImGui::Button("Sphere"))
						{
							shouldCreate = true;
							filepath = "Primitives\\Sphere.fbx";
						}
						if (ImGui::Button("Cylinder"))
						{
							shouldCreate = true;
							filepath = "Primitives\\Cylinder.fbx";
						}
						if (ImGui::Button("Plane"))
						{
							shouldCreate = true;
							filepath = "Primitives\\Plane.fbx";
						}
						if (ImGui::Button("Cone"))
						{
							shouldCreate = true;
							filepath = "Primitives\\Cone.fbx";
						}

						if (shouldCreate)
						{
							Renderable* newRend = nullptr;
							if (nullptr == Entity::getSelected())
								newRend = addEntity<Renderable>(false, newName);
							else
								newRend = Entity::getSelected()->addChild<Renderable>(newName);
							newRend->loadModel(filepath);
							newRend->setEnabled(newEntityEnabled);
							newName = "";
							filepath = "";
							shouldCreate = false;
						}

						ImGui::EndMenu();
					}

					//Custom
					if (ImGui::BeginMenu("Custom"))
					{
						ImGui::InputText("Filepath", &filepath);

						if (ImGui::Button("Create"))
						{
							Renderable* newRend = nullptr;

							if (nullptr == Entity::getSelected())
								newRend = addEntity<Renderable>(false, newName);
							else
								newRend = Entity::getSelected()->addChild<Renderable>(newName);
							newRend->loadModel(filepath);
							newRend->setEnabled(newEntityEnabled);
							newName = "";
							filepath = "";
						}

						ImGui::EndMenu();
					}

					ImGui::Separator();

					// Create Lights
					if (ImGui::BeginMenu("Lights"))
					{
						if (ImGui::Button("Directional"))
						{
							if (nullptr == Entity::getSelected())
								addEntity<Light>(false, "New Light");
							else
								Entity::getSelected()->addChild<Light>("New Light");
						}
						if (ImGui::Button("Point"))
						{
							if (nullptr == Entity::getSelected())
								addEntity<PointLight>(false, "New Light");
							else
								Entity::getSelected()->addChild<PointLight>("New Point Light");
						}
						if (ImGui::Button("Spot"))
						{
							if (nullptr == Entity::getSelected())
								addEntity<SpotLight>(false, "New Light");
							else
								Entity::getSelected()->addChild<SpotLight>("New Spot Light");
						}

						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}
				else
				{
					if (ImGui::IsWindowHovered())
					{
						if (ImGui::IsMouseClicked(0))
							Entity::setSelected(nullptr);
					}
				}

				if (glfwGetKey(TNAP::getApplication()->getWindow(), GLFW_KEY_DELETE) == GLFW_PRESS)
				{
					if (nullptr != Entity::getSelected())
					{
						destroyEntity(Entity::getSelected()->getHandle());
						Entity::setSelected(nullptr);
					}
				}

				ImGui::EndMenuBar();
			}

			ImGui::TextColored(ImVec4(0.9f, 0.49f, 0.17f, 1.0f), m_sceneName.c_str());

			for (std::shared_ptr<Entity>& e : m_entities)
			{
				if (!e->getHasParent())
					e->imGuiRenderHierarchy();
			}
		}
		ImGui::End();

		ImGui::Begin("Entity Properties");
		{
			if (nullptr != Entity::getSelected())
				Entity::getSelected()->imGuiRenderProperties();
		}
		ImGui::End();

		ImGui::Begin("Scene Properties");
		{
			ImGui::ColorPicker3("Ambient Colour", &m_ambientColour.x);
			ImGui::DragFloat("Ambient Intensity", &m_ambientIntensity, 0.1f);
			ImGui::Separator();
			ImGui::DragFloat("Scene Exposure", &m_exposure, 0.1f);
			ImGui::DragFloat("Scene Gamma", &m_gamma, 0.1f);
		}
		ImGui::End();

	}
#endif

}