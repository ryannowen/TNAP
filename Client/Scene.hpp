#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <utility>
#include "Renderable.hpp"

namespace TNAP {

	class Message;
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

		void sendMessage(TNAP::Message* const argMessage);

#if USE_IMGUI
		void imGuiRender();
#endif
	};

	template<class EntityType, typename... Args>
	inline EntityType* const Scene::addEntity(const std::string& argName, const Args&... args)
	{
		if (m_mapEntities.insert({ argName, m_entities.size() }).second)
		{
			m_entities.emplace_back(std::make_shared<EntityType>(args...));
			m_entities.back()->setName(argName);
			return static_cast<EntityType*>(m_entities.back().get());
		}

		return nullptr;
	}

}
