#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <utility>

namespace TNAP {

	class Message;
	class Entity;

	class Scene
	{
	private:
		std::string m_sceneName{ "" };
		std::unordered_map<std::string, size_t> m_mapEntities;
		std::vector<std::shared_ptr<TNAP::Entity>> m_entities;
		std::vector<size_t> m_parentHandles;

		bool m_randomCreationDeletion{ false };

	public:
		Scene(const std::string& argName);
		~Scene();

		void init();
		void update();
		void saveScene();
		void loadFromFile(const std::string& argFilePath);

		template<class EntityType, typename... Args>
		inline EntityType* const addEntity(const bool argIsChild, const std::string& argName, const Args&... args);

		TNAP::Entity* const findEntity(const std::string& argName);
		TNAP::Entity* const findEntity(const size_t argHandle);

		const std::pair<bool, size_t> getEntityIndex(const std::string& argName);
		const bool entityExists(const std::string& argName);
		bool updateEntityInMap(const std::string& argOldName, const std::string& argNewName, const size_t argHandle);

		void destroyEntity(const std::string& argName);
		void destroyEntity(const size_t argHandle);

		void sendMessage(TNAP::Message* const argMessage);

		inline const std::string& getSceneName() const { return m_sceneName; }

#if USE_IMGUI
		void imGuiRender();
#endif
	};

	template<class EntityType, typename... Args>
	inline EntityType* const Scene::addEntity(const bool argIsChild, const std::string& argName, const Args&... args)
	{
		std::string origionalName = argName;
		std::string name = argName;

		if (name.empty())
		{
			name = "New Entity";
			origionalName = name;
		}

		size_t index = 1;
		while (m_mapEntities.find(name) != m_mapEntities.end())
		{
			name = origionalName + "_" + std::to_string(index++);
		}

		if (m_mapEntities.insert({ name, m_entities.size() }).second)
		{
			m_entities.emplace_back(std::make_shared<EntityType>(args...));
			m_entities.back()->setName(name);
			m_entities.back()->setHandle(m_entities.size() - 1);
			m_entities.back()->setHasParent(true);

			if (!argIsChild)
			{
				m_parentHandles.push_back(m_entities.size() - 1);
				m_entities.back()->setParentHandle(m_parentHandles.size()-1);
				m_entities.back()->setHasParent(false);
			}

			return static_cast<EntityType*>(m_entities.back().get());
		}

		return nullptr;
	}

}
