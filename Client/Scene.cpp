#include "Scene.hpp"
#include "Entity.hpp"

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
				entity->update(transform);
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
		// TODO: Swap with back of vector
		// TODO: Pop back of vector
		// TODO: Fix entity handle order in map
	}

	void Scene::destroyEntity(const size_t argHandle)
	{
	}

	void Scene::sendMessage()
	{
	}

#if USE_IMGUI
	void Scene::imGuiRender()
	{
	}
#endif

}