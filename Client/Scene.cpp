#include "Scene.hpp"
#include "Entity.hpp"
#include "ImGuiInclude.hpp"

namespace TNAP {

	Scene::Scene()
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

		Transform transform;

		for (const std::shared_ptr<Entity>& entity : m_entities)
		{
			if (entity->getEnabled())
				entity->update(glm::mat4(1));
		}

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

	void Scene::destroyEntity(const std::string& argName)
	{
		// Invalid entity name
		if (m_mapEntities.find(argName) == m_mapEntities.end())
			return;

		// Get index of entity to be deleted
		size_t entityIndex = m_mapEntities.at(argName);

		// Swap entity to be deleted and the
		// entity at the back of the vector
		std::swap(m_entities[entityIndex], m_entities.back());

		// Update entity handle
		m_mapEntities.at(m_entities[entityIndex]->getName()) = entityIndex;
		m_entities[entityIndex]->setHandle(entityIndex);

		// Erase entity name and handle from map,
		// Pop entity from back of vector
		m_mapEntities.erase(argName);
		m_entities.pop_back();
	}

	void Scene::destroyEntity(const size_t argHandle)
	{
		// Invalid entity handle
		if (argHandle > m_entities.size() || argHandle < 0)
			return;

		// Swap entity to be deleted and the
		// entity at the back of the vector
		std::swap(m_entities[argHandle], m_entities.back());

		// Update entity handle
		m_mapEntities.at(m_entities[argHandle]->getName()) = argHandle;
		m_entities[argHandle]->setHandle(argHandle);

		// Erase entity name and handle from map,
		// Pop entity from back of vector
		m_mapEntities.erase(m_entities.back()->getName());
		m_entities.pop_back();
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
					/*
					if (ImGui::MenuItem("Entity"))
					{
						Entity::getSelected()->children.push_back(std::make_shared<Entity>("New Entity!", std::vector<std::shared_ptr<Entity>>()));
					}
					
					if (ImGui::MenuItem("Player"))
					{
						Entity::selected->children.push_back(std::make_shared<Player>("New Player!", std::vector<std::shared_ptr<Entity>>()));
					}

					if (ImGui::MenuItem("Mesh"))
					{
						Entity::selected->children.push_back(std::make_shared<MeshTest>("New Mesh!", std::vector<std::shared_ptr<Entity>>()));
					}
					*/

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::TextColored(ImVec4(0.9, 0.49, 0.17, 1), "Example Scene Name");

			Entity::setTreeIndex(0);

			for (int i = 0; i < m_entities.size(); i++)
				m_entities[i]->imGuiRenderHierarchy();
		}
		ImGui::End();

		ImGui::Begin("Entity Properties");
		{
			if (nullptr != Entity::getSelected())
				Entity::getSelected()->imGuiRenderProperties();
		}
		ImGui::End();
	}
#endif

}