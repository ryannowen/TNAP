#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace TNAP {
	
	class Entity;

	class Scene
	{
	private:
		std::unordered_map<std::string, size_t> m_mapEntities;
		std::vector<std::shared_ptr<TNAP::Entity>> m_entities;

	public:
		Scene();
		~Scene();

		void init();
		void update();

		template<class EntityType, typename... Args>
		inline EntityType* const addEntity(const std::string& argName, const Args&... args);

		TNAP::Entity* const findEntity(const std::string& argName);
		TNAP::Entity* const findEntity(const size_t argHandle);
		void destroyEntity(const std::string& argName);
		void destroyEntity(const size_t argHandle);
		void sendMessage();

#if USE_IMGUI
		void imGuiRender();
#endif
	};

	template<class EntityType, typename... Args>
	inline EntityType* const Scene::addEntity(const std::string& argName, const Args&... args)
	{

		if (m_mapEntities.find(argName) == m_mapEntities.end())
		{
			m_entities.emplace_back(std::make_shared<EntityType>(args...));
			m_entities.back()->setName(argName);
			m_mapEntities.insert({ argName, m_entities.size() - 1 });
			return m_entities.back().get();
		}

		return nullptr;
	}

}
